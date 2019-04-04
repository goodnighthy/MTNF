import numpy as np
from cycler import cycler
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator, FormatStrFormatter
import matplotlib.patches as mpatches
import brewer2mpl

fig_size = [12, 8]
font_size = 25
# colors = brewer2mpl.get_map('Dark2', 'qualitative', 8).mpl_colors
colors = [
    (74 / 255.0, 114 / 255.0, 176 / 255.0),
    (196 / 255.0, 78 / 255.0, 82 / 255.0),
    (85 / 255.0, 168 / 255.0, 104 / 255.0),
    (129 / 255.0, 114 / 255.0, 178 / 255.0),
    (204 / 255.0, 185 / 255.0, 116 / 255.0),
    (100 / 255.0, 181 / 255.0, 205 / 255.0)
]
plt.rc('axes', prop_cycle=(cycler('color', colors)))
plt.rc('lines', linewidth=5)