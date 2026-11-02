#!/usr/bin/env python3
"""
Convert a Chronos watchface .bin file into LVGL-compatible C assets.

This is a commented Python port of bin2lvgl.kt.  It intentionally keeps the
same file naming, component parsing, resource grouping, and generated C shape
so existing watchface folders can be regenerated with either converter.
"""

from __future__ import annotations

import argparse
import random
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFilter
except ImportError as exc:  # pragma: no cover - this is an environment hint.
    raise SystemExit(
        "bin2lvgl.py needs Pillow for watchface.png rendering. "
        "Install it with: python3 -m pip install Pillow"
    ) from exc


def hex_byte(value: int) -> str:
    """Return the same two-character lowercase hex strings Kotlin produced."""
    return f"{value & 0xFF:02x}"


def u16_le(data: bytes | bytearray, offset: int) -> int:
    """Read a little-endian unsigned 16-bit value."""
    return data[offset] | (data[offset + 1] << 8)


def u32_le(data: bytes | bytearray, offset: int) -> int:
    """Read a little-endian unsigned 32-bit value."""
    return (
        data[offset]
        | (data[offset + 1] << 8)
        | (data[offset + 2] << 16)
        | (data[offset + 3] << 24)
    )


@dataclass
class Item:
    known: bool
    name: str


@dataclass
class Resource:
    id: int
    pos: int


class Show:
    """Preview/update helper for numeric components such as hour digits."""

    def __init__(self, item_id: int, value: int) -> None:
        self.id = item_id
        self.value = value
        self.offset = 0

    def get_off(self, maximum: int) -> int:
        """Choose which frame to draw in the generated PNG preview."""
        digit = self.get_place_value(self.value, self.offset)
        if digit >= maximum:
            digit = 0
        self.offset += 1
        return digit

    @staticmethod
    def get_place_value(number: int, position: int) -> int:
        number_as_string = str(number)
        if position < 0 or position >= len(number_as_string):
            return 0
        return int(number_as_string[len(number_as_string) - 1 - position])

    def get_lv(self, maximum: int) -> str:
        """Return the C expression used to select the LVGL image frame."""
        x = int(10 ** (self.offset - 1))
        if self.id == 0x00:
            return f"(hour / {x}) % {maximum}"
        if self.id == 0x01:
            return f"(minute / {x}) % {maximum}"
        if self.id == 0x02:
            return f"(day / {x}) % {maximum}"
        if self.id == 0x03:
            source = "(month - 1)" if maximum == 12 else "month"
            return f"({source} / {x}) % {maximum}"
        if self.id == 0x06:
            return f"((weekday + 6) / {x}) % {maximum}"
        if self.id == 0x07:
            return f"(year / {x}) % {maximum}"
        if self.id == 0x08:
            return f"(am ? 0 : 1) % {maximum}"
        if self.id == 0x0B:
            return f"(battery / {x}) % {maximum}"
        if self.id == 0x1B:
            return f"(seconds / {x}) % {maximum}"
        if self.id == 0x16:
            return f"(temp / {x}) % {maximum}"
        if self.id == 0x17:
            return "icon % 8"
        if self.id == 0x10:
            return f"(bpm / {x}) % {maximum}"
        if self.id == 0x11:
            return f"(oxygen / {x}) % {maximum}"
        if self.id == 0x0E:
            return f"(steps / {x}) % {maximum}"
        if self.id == 0x0F:
            return f"(kcal / {x}) % {maximum}"
        if self.id == 0x14:
            return f"(distance / {x}) % {maximum}"
        return ""


def group(item_id: int) -> int:
    if item_id in (0x00, 0x01, 0x02, 0x03, 0x06, 0x07, 0x08):
        return 1  # time
    if item_id in (0x0A, 0x0B):
        return 2  # status
    if item_id in (0x0E, 0x0F, 0x14, 0x15):
        return 3  # activity
    if item_id in (0x10, 0x11):
        return 4  # health
    if item_id in (0x26, 0x16, 0x17, 0xFA):
        return 5  # weather
    return -1


def item_type(item_id: int) -> Item:
    names = {
        0x00: (True, "Hour no"),
        0x01: (True, "Minute no"),
        0x02: (True, "Date no"),
        0x03: (True, "Month no"),
        0x06: (True, "Weekday label"),
        0x07: (True, "Year no -"),
        0x08: (True, "AM/PM label"),
        0x09: (True, "Image/Icon"),
        0x0A: (True, "Connection label"),
        0x0B: (True, "Battery no"),
        0x0C: (True, "Sleep label"),
        0x0D: (True, "Analog hands"),
        0x0E: (True, "Steps no"),
        0x0F: (True, "Calories no"),
        0x10: (True, "Heart Rate no"),
        0x11: (True, "SP02 no"),
        0x13: (True, "Sleep no"),
        0x14: (True, "Distance no"),
        0x15: (True, "Distance Label"),
        0x16: (True, "Weather no"),
        0x17: (True, "Weather label"),
        0x19: (True, "Solid color"),
        0x1A: (False, "Analog x5"),
        0x1B: (True, "Seconds"),
        0x1D: (True, "Unknown"),
        0x1E: (False, "Analog x8"),
        0xFA: (True, "Weather no+ label"),
        0xFB: (False, "Click"),
        0xFD: (True, "Animation"),
    }
    known, name = names.get(item_id, (False, f"0x{hex_byte(item_id)} -> \"Unknown\""))
    return Item(known, name)


def subject_name(item_id: int) -> str | None:
    """Map watchface component ids to the global LVGL subject they observe."""
    subjects = {
        0x00: "sb_time_hour",
        0x01: "sb_time_minute",
        0x1B: "sb_time_second",
        0x02: "sb_time_day",
        0x03: "sb_time_month",
        0x06: "sb_time_weekday",
        0x07: "sb_time_year",
        0x08: "sb_time_am",
        0x0A: "sb_system_connection",
        0x0B: "sb_battery_percent",
        0x0E: "sb_activity_steps",
        0x0F: "sb_activity_kcal",
        0x14: "sb_activity_distance",
        0x10: "sb_health_bpm",
        0x11: "sb_health_oxygen",
        0x16: "sb_weather_temp",
        0x17: "sb_weather_code",
    }
    return subjects.get(item_id)


def analog_subject_name(lan: int) -> str | None:
    """Map analog hand sequence positions to rotation subjects."""
    subjects = {
        1: "sb_time_hour_analog",
        17: "sb_time_minute_analog",
        33: "sb_time_seconds_analog",
    }
    return subjects.get(lan)


def frame_order(item_id: int, frame_count: int) -> list[int]:
    """Return generated image-array order for a component.

    Weekday resources are stored Monday-first in the bin, so move the final
    frame (Sunday) to index 0 for subject-driven lookup.
    """
    order = list(range(frame_count))
    if item_id == 0x06 and frame_count > 1:
        return [order[-1], *order[:-1]]
    return order


def generate_list() -> list[Show]:
    """Build preview values used by get_image() and generated update code."""
    now = datetime.now()
    weekday = now.weekday()  # Python already uses Monday=0, Sunday=6.
    return [
        Show(0x00, now.hour),
        Show(0x01, now.minute),
        Show(0x1B, now.second),
        Show(0x02, now.day),
        Show(0x03, now.month),
        Show(0x06, weekday),
        Show(0x07, now.year),
        Show(0x10, 72),
        Show(0x11, 98),
        Show(0x0E, 2735),
        Show(0x0F, 163),
        Show(0x0B, 85),
        Show(0x14, 157),
        Show(0x16, 2222),
        Show(0x13, 3008),
    ]


def rgb565_to_rgba(low: int, high: int, transparent_black: bool = True) -> tuple[int, int, int, int]:
    """Convert one little-endian RGB565 pixel to RGBA8888."""
    r = high & 0xF8
    g = ((high & 0x07) << 5) | ((low & 0xE0) >> 3)
    b = (low & 0x1F) << 3
    alpha = 0 if transparent_black and r == 0 and g == 0 and b == 0 else 255
    return r, g, b, alpha


def get_image(
    rgb565: bytes | bytearray,
    width: int,
    height: int,
    transparent_black: bool = True,
    offset: int = 0,
) -> Image.Image:
    """Create a PIL image from one frame inside a stacked RGB565 resource."""
    image = Image.new("RGBA", (width, height))
    pixels = []
    frame_start = height * width * offset
    for y in range(height):
        for x in range(width):
            j = frame_start + y * width + x
            pixels.append(
                rgb565_to_rgba(
                    rgb565[j * 2],
                    rgb565[j * 2 + 1],
                    transparent_black,
                )
            )
    image.putdata(pixels)
    return image


def buffer_bytes(canvas: Image.Image) -> bytearray:
    """Convert a Pillow RGBA image back to little-endian RGB565 bytes."""
    rgba = canvas.convert("RGBA")
    out = bytearray()
    # Pillow 13 introduced get_flattened_data() as the non-deprecated form.
    pixel_data = (
        rgba.get_flattened_data()
        if hasattr(rgba, "get_flattened_data")
        else rgba.getdata()
    )
    for red, green, blue, alpha in pixel_data:
        red5 = red >> 3
        green6 = green >> 2
        blue5 = blue >> 3
        rgb565_with_alpha = (alpha << 16) | (red5 << 11) | (green6 << 5) | blue5
        out.append(rgb565_with_alpha & 0xFF)
        out.append((rgb565_with_alpha >> 8) & 0xFF)
    return out


def lv_header_bytes(color_format: int, width: int, height: int) -> bytes:
    """Pack LVGL's four-byte binary image header."""
    if not 0 <= color_format <= 31:
        raise ValueError("cf must be between 0 and 31")
    if not 0 <= width <= 2047:
        raise ValueError("w must be between 0 and 2047")
    if not 0 <= height <= 2047:
        raise ValueError("h must be between 0 and 2047")

    header = (
        (color_format & 0x1F)
        | ((0 & 0x07) << 5)
        | ((0 & 0x03) << 8)
        | ((width & 0x07FF) << 10)
        | ((height & 0x07FF) << 21)
    )
    return header.to_bytes(4, "little")


def save_asset(
    rgb565: bytes | bytearray,
    width: int,
    height: int,
    transparent_black: bool,
    amount: int,
    name: str,
    asset: str,
    binary: bool = False,
) -> None:
    """
    Save one stacked resource as LVGL C descriptors or LVGL binary images.

    Watchface bins stack grouped frames vertically.  `amount` says how many
    frames are present, while `height` is the per-frame height.
    """
    out_dir = Path(name)
    if not out_dir.exists():
        out_dir.mkdir(parents=True)
        print("Created output folder")

    text = ASSET_HEADER.replace("{{NAME}}", name.upper()).replace("{{name}}", name.lower())

    for frame in range(amount):
        data_raw = bytearray()
        dat = """
// LVGL 9 format (RGB565) // LVGL_9 compatible 
const LV_ATTRIBUTE_MEM_ALIGN uint8_t face_{{name}}_dial_img_{{asset}}_data_{{frame}}[] = {
{{BYTES}}
    };
        
"""
        dat = (
            dat.replace("{{name}}", name)
            .replace("{{asset}}", asset)
            .replace("{{frame}}", str(frame))
        )

        rgb_bytes = "\t//RGB565 data \n\t"
        alpha_bytes = "\n\n\t// Alpha Channel \n\t"
        rgb_count = 1
        alpha_count = 1

        for y in range(height):
            for x in range(width):
                j = y * width + x + (height * width * frame)
                low = rgb565[j * 2]
                high = rgb565[j * 2 + 1]
                r, g, b, alpha = rgb565_to_rgba(low, high, transparent_black)

                pixel_hex = f"0x{low:02X},0x{high:02X},"
                data_raw.append(high)
                data_raw.append(low)

                if transparent_black:
                    alpha_bytes += f"0x{alpha & 0xFF:02X},"
                    if alpha_count % 64 == 0:
                        alpha_bytes += "\n\t"
                    alpha_count += 1
                    data_raw.append(alpha & 0xFF)

                if rgb_count % 32 == 0:
                    pixel_hex += "\n\t"
                rgb_bytes += pixel_hex
                rgb_count += 1

        dat = dat.replace("{{BYTES}}", rgb_bytes + alpha_bytes)
        text += dat + "\n"

        if transparent_black:
            color, cf = "LV_COLOR_FORMAT_RGB565A8", 5
        else:
            color, cf = "LV_COLOR_FORMAT_RGB565", 4

        text += f"""
const lv_img_dsc_t face_{name}_dial_img_{asset}_{frame} = {{
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = {width},
    .header.h = {height},
    .data_size = sizeof(face_{name}_dial_img_{asset}_data_{frame}),
    .header.cf = {color},
    .data = face_{name}_dial_img_{asset}_data_{frame}}};

    """

        if binary:
            binary_dir = out_dir / "binary"
            if not binary_dir.exists():
                binary_dir.mkdir(parents=True)
                print("Created binary folder")
            (binary_dir / f"{name}_{asset}_{frame}.bin").write_bytes(
                lv_header_bytes(cf, width, height) + data_raw
            )

    if not binary:
        assets_dir = out_dir / "assets"
        if not assets_dir.exists():
            assets_dir.mkdir(parents=True)
            print("Created assets folder")
        (assets_dir / f"face_{name}_dial_img_{asset}.c").write_text(text)


def extract_component_pixels(
    data: bytes,
    descriptor_offset: int,
    item_id: int,
    id2: int,
    x_size: int,
    y_size: int,
    color_table_offset: int,
    data_offset: int,
) -> bytearray:
    """
    Decode a component into little-endian RGB565 bytes.

    Components are either:
    - a solid RGB565 color stored in descriptor bytes 16/17,
    - direct RGB565 pixels when color table and data offsets are identical, or
    - indexed pixels where each source byte selects a two-byte RGB565 palette
      entry from `color_table_offset`.
    """
    output = bytearray()
    for z in range(x_size * y_size):
        if item_id == 0x19 or (item_id == 0x16 and id2 in (0x00, 0x06)):
            output.append(data[descriptor_offset + 16])
            output.append(data[descriptor_offset + 17])
        elif color_table_offset == data_offset:
            output.append(data[(z * 2) + data_offset])
            output.append(data[(z * 2) + data_offset + 1])
        else:
            palette_index = data[z + data_offset]
            palette_offset = color_table_offset + palette_index * 2
            output.append(data[palette_offset])
            output.append(data[palette_offset + 1])
    return output


def find_split_ampm_groups(data: bytes, component_count: int) -> set[int]:
    """Return AM/PM split-pair group numbers that should be generated.

    Some dials contain multiple AM/PM language sets, commonly Chinese first and
    English second.  Split AM/PM sets are encoded as two separate multi-part
    components instead of one vertically stacked resource.  If multiple such
    sets are present, keep only the second one.
    """
    groups: list[int] = []
    lan = 0
    previous_type = 0
    component_group_no = 0

    for x in range(component_count):
        base = x * 20
        item_id = data[base + 4]
        is_multi = (data[base + 7] & 0x80) == 0x80
        component_group_size = (data[base + 7] & 0x7F) if is_multi else 0
        is_grouped = (data[base + 5] & 0x80) == 0x80
        split_ampm = item_id == 0x08 and is_multi and not is_grouped and component_group_size == 2

        if is_multi:
            lan += 1

        if previous_type == 0x09 and item_id == 0x09:
            component_group_no += 1
        elif previous_type != item_id:
            previous_type = item_id
            component_group_no += 1
        elif lan == 1:
            component_group_no += 1

        if split_ampm and lan == 1:
            groups.append(component_group_no)

        if is_multi and lan == component_group_size:
            lan = 0

    if len(groups) > 1:
        return {groups[1]}
    return set(groups)


def extract_components(
    data: bytes,
    name: str,
    face_name: str,
    binary: bool,
    width: int = 360,
    height: int = 360,
    corner_radius: int | None = None,
) -> None:
    component_count = u32_le(data, 0)
    print(f"Detected {component_count} components")

    if component_count > 100:
        print("Watchface not valid, exiting")
        return

    elements = ""
    item_show = generate_list()
    canvas = Image.new("RGBA", (width, height), (0, 0, 0, 255))
    resources: list[Resource] = []

    lan = 0
    text = "Components List\n"
    component_group_no = 0
    weather_count = 0
    previous_type = 0

    use_raw = binary
    if use_raw:
        print("Watchface images will be exported as .bin files, you need to upload them manually")
    else:
        print("Watchface images will be included in the code as .c files")

    r_prefix = "S:"  # LVGL drive-letter path used for raw .bin images.

    extern = ""
    objects = ""
    declare = ""
    face_items = ""
    rsc_array = ""
    rsc_path_array = ""
    lv_update_time = ""
    lv_update_weather = ""
    lv_update_status = ""
    lv_update_activity = ""
    lv_update_health = ""
    seconds_type = "NULL"

    weather_ic = f"static const void *face_{name}_dial_img_weather[] = {{\n"
    weather_path_ic = f"const char *face_{name}_dial_img_weather[] = {{\n"
    conn_ic = f"static const void *face_{name}_dial_img_connection[] = {{\n"
    conn_path_ic = f"const char *face_{name}_dial_img_connection[] = {{\n"
    weather_json = "["
    conn_json = "["
    split_ampm_pairs = {}
    split_ampm_groups_to_use = find_split_ampm_groups(data, component_count)

    for x in range(component_count):
        base = x * 20
        hex_id = "".join(hex_byte(data[base + i]) for i in range(4, 8))
        item_id = data[base + 4]
        x_off = u16_le(data, base + 8)
        y_off = u16_le(data, base + 10)
        x_size = u16_le(data, base + 12)
        y_size = u16_le(data, base + 14)
        color_table_offset = u32_le(data, base + 16)
        data_offset = u32_le(data, base + 20)
        id2 = data[base + 5]

        # Bit 7 of byte 7 marks multi-part resources such as analog hands.
        is_multi = (data[base + 7] & 0x80) == 0x80
        component_group_size = (data[base + 7] & 0x7F) if is_multi else 0

        # Bit 7 of byte 5 marks a vertically stacked resource group.  Some
        # AM/PM faces instead store AM and PM as two separate multi-part
        # components; those must not be split by height.
        is_grouped = (data[base + 5] & 0x80) == 0x80
        split_ampm = item_id == 0x08 and is_multi and not is_grouped and component_group_size == 2
        if item_id == 0x08 and not split_ampm:
            is_grouped = True
        frame_count = (data[base + 5] & 0x7F) if is_grouped else 1
        pivot_offset = data[base + 6]

        if is_multi:
            lan += 1

        if previous_type == 0x09 and item_id == 0x09:
            component_group_no += 1
        elif previous_type != item_id:
            previous_type = item_id
            component_group_no += 1
        elif lan == 1:
            component_group_no += 1

        info = item_type(item_id)
        text += (
            f"{x}\t{component_group_no}\t{lan}\t{hex_id}\t{x_off}\t{y_off}\t"
            f"{x_size}\t{y_size}\t{color_table_offset}\t{data_offset}\t{info.name}\n"
        )

        if split_ampm and component_group_no not in split_ampm_groups_to_use:
            if lan == component_group_size:
                lan = 0
            continue

        if not info.known:
            print(f"Skipping: {hex_id} {info.name}")
            continue

        clt_dat = data_offset
        # print(f"{clt_dat}")

        try:
            output = extract_component_pixels(
                data,
                base,
                item_id,
                id2,
                x_size,
                y_size,
                color_table_offset,
                data_offset,
            )
        except Exception as error:  # Keep converting later components if one is malformed.
            print(f"error at {x} -> {error}")
            continue

        if x_size <= 0 or y_size <= 0:
            continue

        resource = next((r for r in resources if r.id == clt_dat), None)
        resource_json = ""
        resource_pos = resource.pos if resource is not None else x
        if resource is None:
            print(f"rs is null at clt_dat {clt_dat}, using x {x}")

        drawable = (lan in (1, 17, 33)) if item_id == 0x0D else True

        if resource is None and drawable:
            # print(f"Resource {clt_dat} {x}")
            resources.append(Resource(clt_dat, x))

            rsc_arr = f"static const void *face_{name}_dial_img_{x}_{clt_dat}_group[] = {{\n"
            rsc_path_arr = f"const char *face_{name}_dial_img_{x}_{clt_dat}_group[] = {{\n"
            resource_json = "["

            ordered_frames = frame_order(item_id, frame_count)
            for aa in ordered_frames:
                declare += f"LV_IMG_DECLARE(face_{name}_dial_img_{x}_{clt_dat}_{aa});\n"
                rsc_arr += f"\t&face_{name}_dial_img_{x}_{clt_dat}_{aa},\n"
                rsc_path_arr += f'\t"{r_prefix}{name}_{x}_{clt_dat}_{aa}.bin",\n'
                resource_json += f'"{r_prefix}{name}_{x}_{clt_dat}_{aa}.bin", '

            rsc_arr += "};\n"
            rsc_path_arr += "};\n"
            resource_json = resource_json[:-2] + "]"

            if item_id == 0x17:
                weather_ic += f"\t&face_{name}_dial_img_{x}_{clt_dat}_0,\n"
                weather_path_ic += f'\t"{r_prefix}{name}_{x}_{clt_dat}_0.bin",\n'
                weather_json += f'"{r_prefix}{name}_{x}_{clt_dat}_0.bin",'
            elif item_id == 0x0A:
                conn_ic += f"\t&face_{name}_dial_img_{x}_{clt_dat}_0,\n"
                conn_path_ic += f'\t"{r_prefix}{name}_{x}_{clt_dat}_0.bin",\n'
                conn_json += f'"{r_prefix}{name}_{x}_{clt_dat}_0.bin",'
                if conn_ic.count("\n") > 2:
                    rsc_array += conn_ic + "};\n"
                    rsc_path_array += conn_path_ic + "};\n"
                    resource_json = conn_json[:-1] + "]"
            elif frame_count == 1:
                pass
            else:
                rsc_array += rsc_arr
                rsc_path_array += rsc_path_arr

            per_frame_height = y_size // frame_count
            save_asset(
                output,
                x_size,
                per_frame_height,
                not (x == 0 and item_id in (0x09, 0x19)),
                frame_count,
                name,
                f"{x}_{clt_dat}",
                use_raw,
            )
        elif item_id == 0x16 and id2 == 0x00:
            per_frame_height = y_size // frame_count
            save_asset(
                output,
                x_size,
                per_frame_height,
                not (x == 0 and item_id in (0x09, 0x19)),
                frame_count,
                name,
                f"{x}_{clt_dat}",
                use_raw,
            )
            resource_json = "["
            for aa in frame_order(item_id, frame_count):
                resource_json += f'"{r_prefix}{name}_{resource_pos}_{clt_dat}_{aa}.bin", '
            resource_json = resource_json[:-2] + "]"
        else:
            resource_json = "["
            for aa in frame_order(item_id, frame_count):
                resource_json += f'"{r_prefix}{name}_{resource_pos}_{clt_dat}_{aa}.bin", '
            resource_json = resource_json[:-2] + "]"

        if frame_count <= 1:
            resource_json = "null"

        split_ampm_array_name = None
        split_ampm_image_count = 0
        if split_ampm:
            split_ref = {
                "c": f"&face_{name}_dial_img_{x}_{clt_dat}_0",
                "path": f"{r_prefix}{name}_{x}_{clt_dat}_0.bin",
            }
            if lan == 1:
                split_ampm_pairs[component_group_no] = split_ref
            elif lan == component_group_size:
                first_ref = split_ampm_pairs.pop(component_group_no, None)
                if first_ref is not None:
                    split_ampm_array_name = f"face_{name}_dial_img_{x}_{clt_dat}_ampm_group"
                    split_ampm_image_count = 2
                    rsc_array += (
                        f"static const void *{split_ampm_array_name}[] = {{\n"
                        f"\t{first_ref['c']},\n"
                        f"\t{split_ref['c']},\n"
                        "};\n"
                    )
                    rsc_path_array += (
                        f"const char *{split_ampm_array_name}[] = {{\n"
                        f"\t\"{first_ref['path']}\",\n"
                        f"\t\"{split_ref['path']}\",\n"
                        "};\n"
                    )
                    resource_json = f"[\"{first_ref['path']}\", \"{split_ref['path']}\"]"

        if item_id == 0x0A and conn_ic.count("\n") < 3:
            continue

        if is_multi:
            if lan == component_group_size:
                lan = 0
            elif item_id == 0x0D and lan in (1, 32, 40, 17, 33):
                y_off -= y_size - pivot_offset
                x_off -= pivot_offset
            else:
                continue

        if item_id == 0x17:
            weather_count += 1
            if weather_count == 9:
                rsc_array += weather_ic + "};\n"
                rsc_path_array += weather_path_ic + "};\n"
                resource_json = weather_json[:-1] + "]"
            if weather_count != 1:
                continue

        if item_id == 0x16 and id2 == 0x06:
            continue

        show = next((s for s in item_show if s.id == item_id), None)
        place_value = show.offset if show is not None else 0
        offs = show.get_off(frame_count) if show is not None else random.randrange(max(frame_count, 1))
        lv_t = show.get_lv(frame_count) if show is not None else ""
        subject = subject_name(item_id)
        use_wf_item = (
            drawable
            and subject is not None
            and (
                frame_count > 1
                or item_id in (0x0A, 0x17)
                or split_ampm_array_name is not None
            )
            # These two component variants need behavior that the simple
            # place-value widget API does not describe yet.
            and not (item_id == 0x0B and pivot_offset == 0)
            and not (item_id == 0x16 and id2 == 0x00)
        )
        image_array = None
        image_count = frame_count
        analog_subject = analog_subject_name(lan) if item_id == 0x0D else None
        use_wf_analog = drawable and analog_subject is not None

        if use_wf_item:
            if split_ampm_array_name is not None:
                image_array = split_ampm_array_name
                image_count = split_ampm_image_count
            elif item_id == 0x17:
                image_array = f"face_{name}_dial_img_weather"
                image_count = 8
            elif item_id == 0x0A:
                image_array = f"face_{name}_dial_img_connection"
                image_count = 2
            else:
                image_array = f"face_{name}_dial_img_{resource_pos}_{clt_dat}_group"

        if drawable:
            extern += f"\textern lv_obj_t *face_{name}_{x}_{clt_dat};\n"
            objects += f"lv_obj_t *face_{name}_{x}_{clt_dat};\n"
            elements += (
                JSON_ELEM.replace("{{id}}", str(item_id))
                .replace("{{sub}}", str(item_id))
                .replace("{{x}}", str(x_off))
                .replace("{{y}}", str(y_off))
                .replace("{{pvX}}", str(pivot_offset))
                .replace("{{pvY}}", str(y_size - pivot_offset))
                .replace("{{image}}", f"{r_prefix}{name}_{resource_pos}_{clt_dat}_0.bin")
                .replace("{{group}}", resource_json)
            )

            resource_ref = (
                f'"{r_prefix}{name}_{resource_pos}_{clt_dat}_0.bin"'
                if use_raw
                else f"&face_{name}_dial_img_{resource_pos}_{clt_dat}_0"
            )
            if use_wf_analog:
                face_items += (
                    WF_ANALOG.replace("{{PARENT}}", f"face_{name}")
                    .replace("{{CHILD}}", f"face_{name}_{x}_{clt_dat}")
                    .replace("{{CHILD_X}}", str(x_off))
                    .replace("{{CHILD_Y}}", str(y_off))
                    .replace("{{RESOURCE}}", resource_ref)
                    .replace("{{SUBJECT}}", analog_subject)
                    .replace("{{PIVOT_X}}", str(pivot_offset))
                    .replace("{{PIVOT_Y}}", str(y_size - pivot_offset))
                )
            elif use_wf_item:
                face_items += (
                    WF_ITEM.replace("{{PARENT}}", f"face_{name}")
                    .replace("{{CHILD}}", f"face_{name}_{x}_{clt_dat}")
                    .replace("{{CHILD_X}}", str(x_off))
                    .replace("{{CHILD_Y}}", str(y_off))
                    .replace("{{PLACE_VALUE}}", str(place_value))
                    .replace("{{IMAGE_ARRAY}}", image_array or "NULL")
                    .replace("{{IMAGE_COUNT}}", str(image_count))
                    .replace("{{SUBJECT}}", subject)
                )
            else:
                face_items += (
                    LV_ITEM.replace("{{PARENT}}", f"face_{name}")
                    .replace("{{CHILD}}", f"face_{name}_{x}_{clt_dat}")
                    .replace("{{CHILD_X}}", str(x_off))
                    .replace("{{CHILD_Y}}", str(y_off))
                    .replace("{{RESOURCE}}", resource_ref)
                )

            if item_id == 0x0D and not use_wf_analog:
                face_items += f"\tlv_img_set_pivot(face_{name}_{x}_{clt_dat}, {pivot_offset}, {y_size - pivot_offset});\n"
                if lan == 1:
                    lv_update_time += (
                        f"\tlv_img_set_angle(face_{name}_{x}_{clt_dat}, "
                        "hour * 300 + (minute * 5) + (second * (5 / 60)));\n"
                    )
                if lan == 17:
                    lv_update_time += (
                        f"\tlv_img_set_angle(face_{name}_{x}_{clt_dat}, "
                        "(minute * 60) + second);\n"
                    )
            if item_id == 0x0D and lan == 33:
                seconds_type = f"&face_{name}_{x}_{clt_dat}"

        if item_id == 0x16 and id2 == 0x00:
            lv_update_weather += (
                f"\tif (temp >= 0)\n\t{{\n\t\tlv_obj_add_flag(face_{name}_{x}_{clt_dat}, "
                f"LV_OBJ_FLAG_HIDDEN);\n\t}} else {{\n\t\tlv_obj_clear_flag(face_{name}_{x}_{clt_dat}, "
                "LV_OBJ_FLAG_HIDDEN);\n\t}\n"
            )
            continue
        if item_id == 0x16 and id2 == 0x01:
            continue

        if lv_t and not use_wf_item and not (item_id == 0x0B and pivot_offset == 0):
            update_line = (
                f"\tlv_img_set_src(face_{name}_{x}_{clt_dat}, "
                f"face_{name}_dial_img_{resource_pos}_{clt_dat}_group[{lv_t}]);\n"
            )
            item_group = group(item_id)
            if item_group == 1:
                lv_update_time += update_line
            elif item_group == 2:
                lv_update_status += update_line
                if lv_t == "(battery / 100) % 10":
                    lv_update_status += (
                        f"\tif (battery < 100)\n\t{{\n\t\tlv_obj_add_flag(face_{name}_{x}_{clt_dat}, "
                        f"LV_OBJ_FLAG_HIDDEN);\n\t}} else {{\n\t\tlv_obj_clear_flag(face_{name}_{x}_{clt_dat}, "
                        "LV_OBJ_FLAG_HIDDEN);\n\t}\n"
                    )
                    continue
            elif item_group == 3:
                lv_update_activity += update_line
            elif item_group == 4:
                lv_update_health += update_line
            elif item_group == 5:
                lv_update_weather += update_line

        if item_id == 0x17 and not use_wf_item:
            lv_update_weather += (
                f"\tlv_img_set_src(face_{name}_{x}_{clt_dat}, "
                f"face_{name}_dial_img_weather[icon % 8]);\n"
            )
        if item_id == 0x0B and pivot_offset == 0 and not use_wf_item:
            lv_update_status += (
                f"\tlv_img_set_src(face_{name}_{x}_{clt_dat}, "
                f"face_{name}_dial_img_{resource_pos}_{clt_dat}_group[(battery / (100 / {frame_count})) % {frame_count}]);\n"
            )
        if item_id == 0x0A and not use_wf_item:
            lv_update_status += (
                f"\tlv_img_set_src(face_{name}_{x}_{clt_dat}, "
                f"face_{name}_dial_img_connection[(connection ? 0 : 1) % 2]);\n"
            )
        if item_id == 0x08 and not use_wf_item:
            lv_update_time += (
                f"\tif (mode)\n\t{{\n\t\tlv_obj_add_flag(face_{name}_{x}_{clt_dat}, "
                f"LV_OBJ_FLAG_HIDDEN);\n\t}} else {{\n\t\tlv_obj_clear_flag(face_{name}_{x}_{clt_dat}, "
                "LV_OBJ_FLAG_HIDDEN);\n\t}\n"
            )
            lv_update_time += (
                f"\tlv_img_set_src(face_{name}_{x}_{clt_dat}, "
                f"face_{name}_dial_img_{resource_pos}_{clt_dat}_group[(am ? 0 : 1) % 2]);\n"
            )

        if item_id == 0x0D and lan in (17, 33):
            continue
        if x_size > 500 or y_size > 5000:
            print(f"Not valid x->{x_size} y->{y_size}")
        else:
            image = get_image(output, x_size, y_size // frame_count, offset=offs)
            # paste() clips off-canvas positions like Java Graphics.drawImage().
            canvas.paste(image, (x_off, y_off), image)

    out_dir = Path(name)
    if not out_dir.exists():
        out_dir.mkdir(parents=True)
        print("Created output folder")

    (out_dir / "items.txt").write_text(text)

    if use_raw:
        json_file = out_dir / f"{name.lower()}.json"
        json_elements = elements[:-1]
        json_file.write_text(
            JSON_OBJ.replace("{{name}}", name.lower()).replace("{{elements}}", json_elements)
        )

    watchface_path = out_dir / "watchface.png"
    preview_path = out_dir / "preview.png"
    canvas.save(watchface_path)

    # The LVGL preview asset and the saved preview.png are derived from the
    # full watchface render, with a glow border added by process_preview().
    preview = process_preview(watchface_path, preview_path, canvas.size, corner_radius=corner_radius)
    save_asset(buffer_bytes(preview), preview.width, preview.height, False, 1, name, "preview", False)

    declare += f"LV_IMG_DECLARE(face_{name}_dial_img_preview_0);\n"
    if use_raw:
        declare = f"LV_IMG_DECLARE(face_{name}_dial_img_preview_0);\n"
    else:
        # Kept to match the Kotlin converter's generated declarations exactly.
        declare += f"LV_IMG_DECLARE(face_{name}_dial_img_preview_0);\n"

    header = (
        H_FILE.replace("{{NAME}}", name.upper())
        .replace("{{name}}", name.lower())
        .replace("{{EXTERN}}", extern)
        .replace("{{DECLARE}}", declare)
        .replace("{{FACE_NAME}}", face_name)
    )
    source = (
        C_FILE.replace("{{NAME}}", name.upper())
        .replace("{{name}}", name.lower())
        .replace("{{OBJECTS}}", objects)
        .replace("{{ITEMS}}", face_items)
        .replace("{{RSC_ARR}}", "" if use_raw else rsc_array)
        .replace("{{RSC_PATH_ARR}}", rsc_path_array if use_raw else "")
        .replace("{{TIME}}", lv_update_time)
        .replace("{{STATUS}}", lv_update_status)
        .replace("{{WEATHER}}", lv_update_weather)
        .replace("{{ACTIVITY}}", lv_update_activity)
        .replace("{{HEALTH}}", lv_update_health)
        .replace("{{FACE_NAME}}", face_name)
        .replace("{{SECOND}}", seconds_type)
    )

    (out_dir / f"{name}.h").write_text(header)
    (out_dir / f"{name}.c").write_text(source)


JSON_ELEM = """
		{
			"id": {{id}},
			"x": {{x}},
			"y": {{y}},
            "pvX": {{pvX}},
			"pvY": {{pvY}},
			"image": "{{image}}",
			"group": {{group}}
		},"""

JSON_OBJ = """
{
	"name": "{{name}}",
	"elements": [
		{{elements}}
	]
}
"""

LV_ITEM = """
    lv_obj_t *{{CHILD}} = lv_img_create({{PARENT}});
    lv_img_set_src({{CHILD}}, {{RESOURCE}});
    lv_obj_set_width({{CHILD}}, LV_SIZE_CONTENT);
    lv_obj_set_height({{CHILD}}, LV_SIZE_CONTENT);
    lv_obj_set_x({{CHILD}}, {{CHILD_X}});
    lv_obj_set_y({{CHILD}}, {{CHILD_Y}});
"""

WF_ITEM = """
    lv_obj_t *{{CHILD}} = wf_item_create({{PARENT}});
    wf_item_set_place_value({{CHILD}}, {{PLACE_VALUE}});
    wf_item_set_image_array({{CHILD}}, {{IMAGE_ARRAY}}, {{IMAGE_COUNT}});
    wf_item_bind_subject({{CHILD}}, &{{SUBJECT}});
    lv_obj_set_x({{CHILD}}, {{CHILD_X}});
    lv_obj_set_y({{CHILD}}, {{CHILD_Y}});
"""

WF_ANALOG = """
    lv_obj_t *{{CHILD}} = wf_analog_create({{PARENT}});
    wf_analog_set_src({{CHILD}}, {{RESOURCE}});
    wf_analog_bind_rotation({{CHILD}}, &{{SUBJECT}});
    wf_analog_set_pivot_x({{CHILD}}, {{PIVOT_X}});
    wf_analog_set_pivot_y({{CHILD}}, {{PIVOT_Y}});
    lv_obj_set_x({{CHILD}}, {{CHILD_X}});
    lv_obj_set_y({{CHILD}}, {{CHILD_Y}});
"""

H_FILE = """
// File generated by bin2lvgl.py
// developed by fbiego. 
// https://github.com/fbiego
// Watchface: {{NAME}}

#ifndef _FACE_{{NAME}}_H
#define _FACE_{{NAME}}_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#include "../../../../helios_ui.h"
#include "../../../../custom/watchfaces/watchface_manager.h"

#ifndef ENABLE_FACE_{{NAME}}
#define ENABLE_FACE_{{NAME}}
#endif

#ifdef ENABLE_FACE_{{NAME}}

{{DECLARE}}

#endif

lv_obj_t *init_face_{{name}}(lv_obj_t *parent);
    

#ifdef __cplusplus
}
#endif

#endif
"""

C_FILE = """
// File generated by bin2lvgl.py
// developed by fbiego. 
// https://github.com/fbiego
// Watchface: {{NAME}}

#include "{{name}}.h"

#ifdef ENABLE_FACE_{{NAME}}

{{RSC_ARR}}

{{RSC_PATH_ARR}}

HELIOS_REGISTER_WATCHFACE(
    "{{FACE_NAME}}",
    "{{NAME}}",
    init_face_{{name}},
    &face_{{name}}_dial_img_preview_0
);

#endif

lv_obj_t *init_face_{{name}}(lv_obj_t *parent){
    lv_obj_t *face_{{name}} = lv_obj_create(parent);
#ifdef ENABLE_FACE_{{NAME}}
    
    lv_obj_remove_style_all(face_{{name}});
    lv_obj_set_size(face_{{name}}, lv_pct(100),lv_pct(100));
    lv_obj_set_flag(face_{{name}}, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_flag(face_{{name}}, LV_OBJ_FLAG_CLICKABLE, false);

    {{ITEMS}}

#endif
    return face_{{name}};
}

"""

ASSET_HEADER = """
// File generated by bin2lvgl
// developed by fbiego. 
// https://github.com/fbiego
// Watchface: {{NAME}}

#include "../{{name}}.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

"""


def parse_bool(value: str | None) -> bool:
    if value is None:
        return False
    return value.lower() == "true"

def process_preview(
    src_path,
    dest_path,
    full_canvas_size,
    glow_color=(255, 255, 255, 255),
    glow_radius=10,
    corner_radius=None,
    antialias_scale=4,
):
    """Create and save the generated watchface preview with a soft glow edge."""
    preview_scale = 0.75
    final_size = (
        max(1, int(full_canvas_size[0] * preview_scale)),
        max(1, int(full_canvas_size[1] * preview_scale)),
    )
    if corner_radius is None:
        corner_radius = min(final_size) // 2
    else:
        # The input radius describes the full-size display corners, so scale it
        # down with the preview dimensions.
        corner_radius = max(0, int(corner_radius * preview_scale))
    antialias_scale = max(1, int(antialias_scale))
    resampling = getattr(Image, "Resampling", Image).BILINEAR

    # 1. Load and shrink the original image to make room for glow
    img = Image.open(src_path).convert("RGBA")
    shrink = glow_radius  # shrink margin on each side
    inner_size = (
        max(1, final_size[0] - 2 * shrink),
        max(1, final_size[1] - 2 * shrink),
    )
    img = img.resize(inner_size, resampling)

    # 2. Create rounded mask for the shrunken image
    mask_size = (inner_size[0] * antialias_scale, inner_size[1] * antialias_scale)
    mask = Image.new("L", mask_size, 0)
    draw = ImageDraw.Draw(mask)
    draw.rounded_rectangle(
        [(0, 0), (mask_size[0] - 1, mask_size[1] - 1)],
        radius=max(0, corner_radius - shrink) * antialias_scale,
        fill=255,
    )
    mask = mask.resize(inner_size, resampling)
    img.putalpha(mask)

    # 3. Create full-size canvas and glow layer
    canvas = Image.new("RGBA", final_size, (0, 0, 0, 0))

    # Build the glow from the same mask as the image.  This keeps the glow
    # thickness even on non-circular rounded corners.
    glow_mask = Image.new("L", final_size, 0)
    glow_mask.paste(mask, (shrink, shrink))

    # Blur for glow softness
    blurred = glow_mask.filter(ImageFilter.GaussianBlur(glow_radius / 2))
    glow = Image.new("RGBA", final_size, glow_color)
    glow.putalpha(blurred)

    # 4. Composite glow and centered image
    result = Image.alpha_composite(canvas, glow)
    result.paste(img, (shrink, shrink), img)

    # 5. Save the preview as PNG over a black background.  Keeping it opaque
    # avoids transparent previews showing different colors in file browsers.
    black_background = Image.new("RGBA", final_size, (0, 0, 0, 255))
    result = Image.alpha_composite(black_background, result)
    result.save(dest_path, format="PNG")
    return result

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert a watchface .bin into LVGL C code and preview images."
    )
    parser.add_argument("bin_file", help="Input watchface .bin file")
    parser.add_argument("face_name", nargs="?", help="Display name for the generated face")
    parser.add_argument(
        "binary",
        nargs="?",
        default="false",
        help="true to emit LVGL .bin image assets instead of embedding image C files",
    )
    parser.add_argument("--width", type=int, default=360, help="Preview canvas width")
    parser.add_argument("--height", type=int, default=360, help="Preview canvas height")
    parser.add_argument(
        "--corner-radius",
        type=int,
        default=None,
        help="Full-size display corner radius; scaled to the 75%% preview size",
    )
    args = parser.parse_args()

    input_path = Path(args.bin_file)
    data = input_path.read_bytes()
    if not data:
        print(f"Could not read from {args.bin_file}")
        return

    # Match Kotlin's name cleanup, but use only the file name so paths do not
    # become part of generated C identifiers.
    name = input_path.name.replace(".bin", "").replace("-", "_").replace("_dial", "")
    face_name = args.face_name if args.face_name is not None else name.replace("_", " ")
    extract_components(
        data,
        name,
        face_name,
        parse_bool(args.binary),
        args.width,
        args.height,
        args.corner_radius,
    )
    print("-----Done-------")


if __name__ == "__main__":
    main()
