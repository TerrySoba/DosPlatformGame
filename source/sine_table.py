import math


count = 256
value = (2 * math.pi) / count

print("int8_t s_sineTable[] = {")
for i in range(count):
    print("{},".format(round(math.sin(value * i) * 127)), end="")
print("}")

