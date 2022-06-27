import os
import argparse
import sys
import cairo
import re
import colorsys

def make_sure_path_exists(path):
    try:
        os.makedirs(path)
    except FileExistsError as exception:
        pass
    pass

class ReadGlobals():
    def __init__(self,base_file_name):
        self._colors = {}
        self._colors = self.read_globals(base_file_name)

    def read_globals(self,filename):
        with open(filename,'r',encoding="utf-8") as lines:
            while True:
                prefix_name = str()
                for line in lines:
                    match_groups = re.search('([a-zA-Z]+)\]$', line)
                    prefixs = ['Radius', 'Disabled', 'Inactive', 'Default', 'Widget', 'Bare', 'Window', 'Selection', 'View', 'WM','SpecialButton']
                    if match_groups != None and len(match_groups.groups()) == 1 and match_groups.groups()[0] in prefixs:
                        prefix_name = match_groups.groups()[0]
                        break
                if len(prefix_name) == 0:
                    break

                for line in lines:
                    if line == '\n':
                        break
                    key = '{0}{1}'.format(prefix_name, line.strip().split('=')[0])
                    value = line.strip().split('=')[1]
                    if value == '':
                        continue
                    self._colors[key] = self.trans_value(value)
        return self._colors

    def _get_function_args(self, func_expr):
        args = list()
        cur_index = func_expr.find('(')
        while cur_index != -1:
            last_index = func_expr.find(',', cur_index + 1)
            if last_index == -1:
                last_index = func_expr.find(')', cur_index + 1)

            if last_index != -1 and last_index > cur_index + 1:
                arg = func_expr[cur_index + 1:last_index].strip()
                if len(arg) > 0:
                    args.append(arg)

            cur_index = last_index
        return args

    def trans_value(self, value):
        # function
        if str.startswith(value, 'lighten'):
            color, amount = self._get_function_args(value)
            color = Color.int_to_rgb(self.trans_value(color))
            return Color.rgb_to_int(Color.lighter(color, float(amount)))
        elif str.startswith(value, 'darken'):
            color, amount = self._get_function_args(value)
            color = Color.int_to_rgb(self.trans_value(color))
            return Color.rgb_to_int(Color.darker(color, float(amount)))
        elif str.startswith(value, 'mix'):
            color1, color2, weight = self._get_function_args(value)
            color1 = Color.int_to_rgb(self.trans_value(color1))
            color2 = Color.int_to_rgb(self.trans_value(color2))
            return Color.rgb_to_int(Color.mix(color1, color2, float(weight)))
        elif str.endswith(value, 'px'):
            return value
        # bool value
        elif value.lower() == 'true' or value.lower() == 'false':
            return bool(value)
        # variable
        elif not re.match('^\d', value):
            return self._colors[value.replace(':', '')]
        # float value
        elif value.find('.') != -1:
            return float(value)
        # color value
        elif value.find(',') != -1:
            r = int(value.split(',')[0], 0)
            g = int(value.split(',')[1], 0)
            b = int(value.split(',')[2], 0)
            return Color.rgb255_to_int((r, g, b))
        # integer value
        else:
            return int(value, 0)
class Color(object):
    def __init__(self, colordict, name, name2=None, amount=0):
        color = colordict[name]
        self.colordict = colordict

        r, g, b = Color.int_to_rgb255(color)
        if name2 is not None:
            color2 = colordict[name2]
            r2, g2, b2 = Color.int_to_rgb255(color2)
            r = r * amount + float(r2) * (1 - amount)
            g = g * amount + float(g2) * (1 - amount)
            b = b * amount + float(b2) * (1 - amount)

        self.rgb255 = (int(r), int(g), int(b))
        self.rgb = (r / 255, g / 255, b / 255)
        self.html = '#%02x%02x%02x' % self.rgb255
        self.insensitive = self._color_effect(self._intensity_effect(self.rgb, 'Disabled'), 'Disabled')
        self.insensitive_alpha = self._contrast_effect(self.rgb, 'Disabled')

        if not self.colordict['InactiveEnable']:
            self.inactive = self.rgb
            self.inactive_alpha = 1.0
        else:
            self.inactive = self._color_effect(self._intensity_effect(self.rgb, 'Inactive'), 'Inactive')
            self.inactive_alpha = self._contrast_effect(self.rgb, 'Inactive')
        self.inactive_insensitive = self._color_effect(self._intensity_effect(self.inactive, 'Disabled'), 'Disabled')
        self.inactive_insensitive_alpha = max(self.inactive_alpha - (1 - self.insensitive_alpha), 0)

    @staticmethod
    def int_to_rgb255(num):
        r = ((num >> 16) & 0xff)
        g = ((num >> 8) & 0xff)
        b = (num & 0xff)
        return (r, g, b)

    @staticmethod
    def int_to_rgb(num):
        r = float((num >> 16) & 0xff) / 255
        g = float((num >> 8) & 0xff) / 255
        b = float(num & 0xff) / 255
        return (r, g, b)

    @staticmethod
    def rgb_to_int(rgb):
        return (int(rgb[0] * 255) << 16) + (int(rgb[1] * 255) << 8) + int(rgb[2] * 255)

    @staticmethod
    def rgb255_to_int(rgb):
        return (rgb[0] << 16) + (rgb[1] << 8) + rgb[2]

    @staticmethod
    def mix(color, mix_color, amount):
        r = color[0] * amount + mix_color[0] * (1 - amount)
        g = color[1] * amount + mix_color[1] * (1 - amount)
        b = color[2] * amount + mix_color[2] * (1 - amount)
        return (r, g, b)

    @staticmethod
    def lighter(color, amount):
        h, s, v = colorsys.rgb_to_hsv(color[0], color[1], color[2])
        v = min((1 + amount) * v, 1)
        r, g, b = colorsys.hsv_to_rgb(h, s, v)
        return (r, g, b)

    @staticmethod
    def darker(color, amount):
        h, s, v = colorsys.rgb_to_hsv(color[0], color[1], color[2])
        if amount == -1:
            v = 1
        else:
            v = min(v / (1 + amount), 1)
        r, g, b = colorsys.hsv_to_rgb(h, s, v)
        return (r, g, b)

    @staticmethod
    def desaturate(color, amount):
        h, s, v = colorsys.rgb_to_hsv(color[0], color[1], color[2])
        s = min(s * (1 - amount), 1)
        r, g, b = colorsys.hsv_to_rgb(h, s, v)
        return (r, g, b)

    def _intensity_effect(self, color, state):
        effect = int(self.colordict[state + 'IntensityEffect'])
        amount = float(self.colordict[state + 'IntensityAmount'])
        if effect == 0:
            (r, g, b) = color
        elif effect == 1:
            if amount >= 0:
                (r, g, b) = self.mix((1.0, 1.0, 1.0), color, amount)
            else:
                (r, g, b) = self.mix((0.0, 0.0, 0.0), color, amount)
        elif effect == 2:
            (r, g, b) = self.darker(color, amount)
        elif effect == 3:
            (r, g, b) = self.lighter(color, amount)
        return (r, g, b)

    def _color_effect(self, color, state):
        effect = int(self.colordict[state + 'ColorEffect'])
        amount = float(self.colordict[state + 'ColorAmount'])
        effect_color = self.int_to_rgb(self.colordict[state + 'Color'])
        if effect == 0:
            (r, g, b) = color
        elif effect == 1:
            (r, g, b) = self.desaturate(color, amount)
        else:
            (r, g, b) = self.mix(effect_color, color, amount)
        return (r, g, b)

    def _contrast_effect(self, color, state):
        effect = int(self.colordict[state + 'ContrastEffect'])
        amount = float(self.colordict[state + 'ContrastAmount'])
        if effect == 0:
            return 1.0
        else:
            return 1.0 - amount

    def lighten_color(self, amount):
        h, s, v = colorsys.rgb_to_hsv(self.rgb[0], self.rgb[1], self.rgb[2])
        v = (1 + amount) * v
        r, g, b = colorsys.hsv_to_rgb(h, s, v)
        self.rgb = (r, g, b)
        self.rgb255 = (int(r * 255), int(g * 255), int(b * 255))

    def gradient(self, state='', alpha=1.0):
        if state == 'active':
            stop1 = self.lighter(self.rgb, 0.03)
            stop2 = self.darker(self.rgb, 0.10)
            linear = cairo.LinearGradient(1, 1, 1, 19)
            linear.add_color_stop_rgba(0.0, stop1[0], stop1[1], stop1[2], alpha)
            linear.add_color_stop_rgba(1.0, stop2[0], stop2[1], stop2[2], alpha)
        else:
            stop1 = self.lighter(self.rgb, 0.01)
            stop2 = self.darker(self.rgb, 0.03)
            linear = cairo.LinearGradient(1, 1, 1, 19)
            linear.add_color_stop_rgba(0.0, stop1[0], stop1[1], stop1[2], alpha)
            linear.add_color_stop_rgba(1.0, stop2[0], stop2[1], stop2[2], alpha)
        return linear
# ___________________________________________________________________________________
parser = argparse.ArgumentParser(description='Generates Kiran assets according to the specified color ' 'scheme.')
parser.add_argument('--colorscheme', '-c', action='store', help='color scheme to use')
parser.add_argument('--basecolorscheme', '-b', action='store', help='base color scheme')
parser.add_argument('--generate-dir', '-d', action='store', help='generate color directory')

args = parser.parse_args()

generate_path = args.generate_dir
make_sure_path_exists(generate_path)

_colors = ReadGlobals(args.basecolorscheme).read_globals(args.colorscheme)

globalScss = open(os.path.join(generate_path, '_global.scss'), 'w')
for key in sorted(_colors):
    if key == 'DisabledColor' or key == 'InactiveColor':
        globalScss.write('${0}: #{1:x};\n'.format(key, _colors[key]))
    elif type(_colors[key]) == type(str()) or ('Disabled' in key or 'Inactive' in key):
        globalScss.write('${0}: {1};\n'.format(key, _colors[key]))
    else:
        globalScss.write('${0}: #{1:x};\n'.format(key, _colors[key]))
globalScss.close()