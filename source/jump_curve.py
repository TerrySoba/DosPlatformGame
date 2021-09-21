
dy = 32
y = 0

old = 0

diffs = []

out = ""

for x in range(50):
    old = y
    y += dy
    out += "{:.0f}, ".format(y - old)
    dy -= (0.65 * 2)
    print("{:1.0f}".format(y))



print(out)