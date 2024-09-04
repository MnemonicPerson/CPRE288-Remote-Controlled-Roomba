with open('values.txt', 'r') as file:
    lines = file.readlines()

lines.pop(0)
formatted_lines = []
for line in lines:
    data = line.split()
    dist = data[0]
    ir_val = data[1]
    s = "\t(ir_dist_lookup) {.distance_cm = "+dist+", .ir_val="+ir_val+"},\n"
    formatted_lines.append(s)

with open('formatted.txt', 'w') as file:
    file.writelines(formatted_lines)
