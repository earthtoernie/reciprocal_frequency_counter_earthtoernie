print("hello world")
from pprint import pprint
o = '_'
x = 'X'
pixels = (
    (1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26),
    (o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o),
    (o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o)
)

print('\n'.join([''.join(['{:4}'.format(str(item)) for item in row]) for row in pixels]))