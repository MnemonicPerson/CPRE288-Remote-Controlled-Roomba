import socket
import numpy as np
import matplotlib.pyplot as plt
from tkinter import *
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg)
import time

# HOST = '192.168.1.1' # ip of the CyBOT (localhost rn)
# PORT = 288 # port to use (65432 used by server code)
# cybot = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# cybot.setblocking(False)
# cybot.settimeout(10)
# cybot.connect((HOST, PORT))
connected = False
# while not connected:
#     try:
#         cybot.connect((HOST, PORT))
#         connected = True
#     except BlockingIOError:
#         # Connection is still in progress, continue looping
#         pass
#     except Exception as e:
#         # Handle other exceptions, like connection refused, etc.
#         # print("Error:", e)
#         break


# angle_degrees: a vector (i.e., array of numbers) for which each element is an angle at which the sensor 
# makes a distance measurement.
# arange: used to to store into vector angle_degrees numbers from 0 degrees to 180 degrees, counting by 4's.
# Units: degrees
angle_degrees = np.arange(0, 360, 1)

# distance: a vector, where each element is the corresponding distance measured at each angle in vector angle_degrees
# Units: meters
global distance
distance = [50.0] * 360

# angle_radians: a vector that contains converted elements of vector angle_degrees into radians 
# Units radians
global angle_radians
angle_radians = (np.pi/180) * angle_degrees

global waiting_for_data
waiting_for_data = False

class KnownObject():

    def __init__(self, angle, distance, radial_width, lin_width):
        # relative to robot
        self.angle = angle
        self.distance = distance

        # TODO
        # relative to robot's starting spot
        # self.cart_x = cart_x
        # self.cart_y = cart_y
        
        self.radial_width = radial_width
        self.lin_width = lin_width

window = Tk()
window.title('CyCmd')
window.geometry("1920x1080")

fig = Figure(figsize=(6, 4))
ax = fig.add_subplot(projection='polar')
ax.set_title("CyBot Radar Scan", size=14, y=1.0, pad=-24) 

canvas = FigureCanvasTkAgg(fig, master=window)
canvas.draw()
canvas.get_tk_widget().pack()

def draw_plot():
    global distance
    global angle_radians
    ax.cla()
    ax.plot(angle_radians, distance, color='r', linewidth=4.0)  # Plot distance verse angle (in radians), using red, line width 4.0
    ax.set_xlabel('Distance (cm)', fontsize = 14.0)  # Label x axis
    ax.set_ylabel('Angle (degrees)', fontsize = 14.0) # Label y axis
    ax.xaxis.set_label_coords(0.5, 0.0) # Modify location of x axis label (Typically do not need or want this)
    ax.tick_params(axis='both', which='major', labelsize=14) # set font size of tick labels
    
    # max_distance = 50
    ax.set_rmax(50)                    # Saturate distance at 50 centimeters
    ax.set_rticks([10, 20, 30, 40, 50])   # Set plot "distance" tick marks at .5, 1, 1.5, 2, and 2.5 meters
    # ax.set_rticks(np.linspace(0, max_distance, 5))
    ax.set_rlabel_position(-22.5)     # Adjust location of the radial labels
    ax.set_thetamax(360)              # Saturate angle to 360 degrees
    ax.set_xticks(np.arange(0,2*np.pi+.1,np.pi/4)) # Set plot "angle" tick marks to pi/4 radians (i.e., displayed at 45 degree) increments
                                                # Note: added .1 to pi to go just beyond pi (i.e., 180 degrees) so that 180 degrees is displayed
    ax.grid(True)                     # Show grid lines
    canvas.draw()

draw_plot()

shellTitle = Label(text="SHELL", font=("Arial", 20))
shellTitle.place(x=0, y=0)
shell = Text(master=window, bg="black", fg="lightgreen", insertbackground="white", height=110, width=25, font=("Arial", 15))
shell.place(x=0,y=50)

shell.insert(END, "> ")
shellLine = 1
global driveMode
driveMode = False

global command_history
command_history = []
global history_index
history_index = 0

# prints only 1 line to shell console
def shell_print(string):
    global shellLine
    shell.delete(f"{shellLine}.0", END)
    shell.insert(END, f"\n{string}\n")
    shell.insert(END, "> ")
    shellLine += 1

def receive_data():
    global waiting_for_data
    waiting_for_data = True
    char = 0
    waited = 0
    while char == 0 and waited <= 7:
        try:
            char = cybot.recv(1).decode()
        except BlockingIOError:
            time.sleep(0.5)
            waited += 0.5
    if char == 0:
        return [] # no data received

    lines = []
    done = False
    string = ""
    waited = 0
    print("Waiting for data")
    while not done and waited <= 7:
        if char == '\n':
            if "DONE" in string:
                # lines.append("DONE")
                done = True
                continue
            print(string)
            lines.append(string)
            string = ""
        else:
            string += char
        try:
            char = cybot.recv(1).decode()
            waited = 0 # reset wait count if we receive data
        except BlockingIOError:
            # print("BlockingIOError")
            time.sleep(0.5)
            waited += 0.5
    
    if waited > 7:
        lines.append("timeout")
        print("Warning! Timeout occured")
    
    waiting_for_data = False
    return lines

            

def run_command(command):
    global cybot
    global command_history
    global distance
    global angle_radians
    global driveMode

    print("Running Command: " + command)
    # clear screen
    if command == "clear":
        shell.delete('1.0', END)
        shell.insert(END, "> ")
        global shellLine
        shellLine = 1
    elif "send" in command:
        data = command.split(' ')

        for i in range(1, len(data)):
            if data[i].isdigit():
                # print(chr(int(data[i])))
                cybot.send(chr(int(data[i])).encode())
            else:
                cybot.send(data[i].encode())

    elif command == "scan":
        # while True:
        # cybot.send("m".encode())
        # shell_print("Running CyBOT scan")

        # lines = []
        # char = 0
        # waited = 0
        # while char == 0 and waited <= 7:
        #     try:
        #         char = cybot.recv(1).decode()
        #     except BlockingIOError:
        #         time.sleep(1)
        #         waited += 1
        #         pass
        # if char == 0:
        #     print("No scan data returned")
        #     return

        # done = False
        # string = ""
        # waited = 0
        # print("Waiting for scan data")
        # while not done and waited <= 7:
        #     # if char == 0:
        #     #     continue
        #     if char == '\n':
        #         if string == "DONE\r":
        #             done = True
        #             continue
        #         print(string)
        #         lines.append(string)
        #         string = ""
        #     else:
        #         string += char
        #     try:
        #         char = cybot.recv(1).decode()
        #         waited = 0
        #     except BlockingIOError:
        #         print("Error. Waiting")
        #         # char = 0
        #         time.sleep(1)
        #         waited += 1
        #         pass
        
        # if waited > 7:
        #     print("Warning! Timeout occured")
        #     return

        # makeshift data for working without robot
        with open('putty.log', 'r') as file:
            lines = file.readlines()
            
        try:
        # print(lines)
            lines = lines[-183:]
            lines = lines[:-1]
        # print(lines)

        # lines = ['     angle (degrees)              IR val\r', '                   0                1006\r', '                   5                 653\r', '                  10                 650\r', '                  15                 590\r', '                  20                 895\r', '                  25                 765\r', '                  30                 741\r', '                  35                 816\r', '                  40                 883\r', '                  45                1001\r', '                  50                1103\r', '                  55                1097\r', '                  60                1211\r', '                  65                1096\r', '                  70                 911\r', '                  75                 781\r', '                  80                 576\r', '                  85                 568\r', '                  90                 569\r', '                  95                 568\r', '                 100                 574\r', '                 105                 565\r', '                 110                 565\r', '                 115                 569\r', '                 120                 571\r', '                 125                 573\r', '                 130                 583\r', '                 135                 584\r', '                 140                 572\r', '                 145                 564\r', '                 150                 563\r', '                 155                 674\r', '                 160                 659\r', '                 165                 583\r', '                 170                 579\r', '                 175                 638\r', '                 180                 550\r', '===\r', 'Results:\r', '===\r', 'Object 1:\r', '\tAngle: 47.00\r', '\tDistance: 1.40\r', '\tRadial Width: 55\r', '\tLinear Width: 134.340576 cm\r']

        # lines = receive_data()
        # if len(lines) == 0 or lines[-1] == "timeout":
        #     return

        # print("received data")

        # for line in lines:
        #     print(line)
            angles = []
            distances = []

            # index = 1 # skip first line
            # while '===' not in lines[index]:
            #     split_line = lines[index].strip().split()
            #     angles.append(split_line[0])
            #     ir_vals.append(split_line[1])
            #     index += 1
            print("new scan data")

            index = 1 # skip first line
            while index < len(lines):
                try:
                    split_line = lines[index].strip().split()
                    angles.append(split_line[0])
                    distances.append(float(split_line[1]))
                except Exception:
                    pass
                index += 1
            # print(angles)
            # print(distances)

            angle_degrees = np.array([round(float(i), 3) for i in angles])

            # index currently is at the start of results
            # index += 3 # gets to 1st object's data
            # objects = []
            # while index < len(lines):
            #     # index += 1
            #     angle = float(lines[index+1].strip().split(' ')[1])
            #     dist = float(lines[index+2].strip().split(' ')[1])
            #     radial_width = float(lines[index+3].strip().split(' ')[2])
            #     lin_width = float(lines[index+4].strip().split(' ')[2])

            #     obj = KnownObject(angle, dist, radial_width, lin_width)
            #     objects.append(obj)
            #     index += 5

            # this is the GLOBAL distance array
            # for i in range(0, 37):
            #     distance[i] = 3.0

            # for obj in objects:
            #     startAngle = obj.angle - (obj.radial_width / 2)
            #     endAngle = obj.angle + (obj.radial_width / 2)

            #     curr = 0
            #     # find closest angles to start and end angle
            #     while angle_degrees[curr] < startAngle:
            #         curr += 1
                
            #     while angle_degrees[curr] < endAngle:
            #         distance[curr] = obj.distance
            #         curr += 1
            
            # distance = distances
            angle_inc = angle_degrees[1] - angle_degrees[0]
            deg = angle_degrees[len(angle_degrees) - 1] + angle_inc
            while deg < 360:
                angle_degrees = np.append(angle_degrees, deg)
                deg += angle_inc

            angle_radians = (np.pi/180) * angle_degrees

            index = 0
            for dist in distances:
                distance[index] = dist
                index += 1

            # print(distance)
            # print(angle_radians)    
            # print(len(distance))
            # print(len(angle_radians))

            draw_plot()
        except:
            pass
    elif "calibrate" in command:
        data = command.split(' ')
        if data[1] == "servo":
            run_command("send c")
        elif data[1] == "turning":
            run_command("send t")
        elif data[1] == "adc":
            run_command("send i")
    elif "echo" in command:
        data = command.split(' ')
        index = 1
        string = ""
        while index < len(data):
            string += data[index] + " "
            index += 1
        shell_print(string.strip())
    elif "turn" in command:
        # should only be between 0 and 180 degrees
        # data = command.split(' ')
        # degrees = int(data[1])

        # if degrees > 0:
        #     run_command(f"send d {degrees}")
        # else:
        #     run_command(f"send a {degrees}")
        data = receive_data()
        if "right" in command:
            if "hard" in command:   # hard right
                degrees = 90
                run_command("send >")
            else:                   # normal right
                degrees = 10
                run_command("send d")
            if len(data) > 0 and data[-1] != "timeout":
                angle_radians = np.roll(angle_radians, int(-degrees))
                draw_plot()
            else:
                print("timeout")
        elif "left" in command:
            if "hard" in command:   # hard left
                degrees = -90
                run_command("send <")
            else:                   # normal left
                degrees = -10
                run_command("send a")
            degrees *= -1
            if len(data) > 0 and data[-1] != "timeout":
                angle_radians = np.roll(angle_radians, int(-degrees))
                draw_plot()
        else:
            print("timeout")
        

        
    # elif "left" in command:
    #     data = command.split(' ')
    #     degrees = int(data[1])
    #     run_command(f"send a {degrees}")

    #     # angle_radians = np.roll(angle_radians, int(degrees / 5))
    #     # draw_plot() 
    # elif "right" in command:
    #     data = command.split(' ')
    #     degrees = int(data[1])
    #     run_command(f"send d {degrees}")

    #     # angle_radians = np.roll(angle_radians, int(degrees / 5))
    #     # draw_plot() 
    elif "forward" in command:
        if "big" in command:        # big forward
            run_command("send g")
        else:
            run_command("send w")   # normal forward
        receive_data()
        # done = False
        # string = ""
        # waited = 0
        # char = 0
        # while char == 0 and waited <= 7:
        #     try:
        #         char = cybot.recv(1).decode()
        #     except BlockingIOError:
        #         time.sleep(1)
        #         waited += 1
        #         pass
        # if char == 0:
        #     print("Inconclusive drive data returned. Scan again")
        # while not done and waited <= 7:
        #     if char == '\n':
        #         if string == "DONE\r":
        #             done = True
        #             continue
        #         print(string)
        #         string = ""
        #     else:
        #         string += char
        #     try:
        #         char = cybot.recv(1).decode()
        #     except BlockingIOError:
        #         time.sleep(1)
        #         waited += 1
        #         pass
        
        # if waited > 10:
        #     print("Warning! Timeout")
    elif "backward" in command:

        # data = command.split(' ')
        # distance = float(data[1])
        # distance = int(distance * 10)

        run_command("send s")
        receive_data()
    elif command == "drive":
        driveMode = True
        shell_print("-Drive Mode Activated-")
        shell_print("Press 'q' to quit")
        shell_print("")
        shell.delete(f"{shellLine}.0", END)
    elif "music" in command:
        run_command("send p")
    elif command == "quit":
        window.destroy()
        cybot.close()
        quit()

def keypress(event):
    global shellLine
    global history_index
    global driveMode
    global waiting_for_data

    # if waiting_for_data:
    #     return # don't do anything

    if event.widget == shell:
        if driveMode:
            shell.mark_set("insert", f"{shellLine}.0 lineend")
            if event.keysym == "Up":
                run_command("forward")
            elif event.keysym == "Left":
                run_command("turn left")
            elif event.keysym == "Down":
                run_command("backward")
            elif event.keysym == "Right":
                run_command("turn right")
            elif event.keysym == "f":
                run_command("big forward")
            elif event.keysym == "l":
                run_command("turn hard left")
            elif event.keysym == "r":
                run_command("turn hard right")
            elif event.keysym == "m":
                run_command("scan")
            elif event.keysym == "p":
                run_command("music")
            elif event.char == "q":
                driveMode = False
                shell_print("")
            return
        # gets command that was typed
        if event.keysym == "Return":
            command = shell.get(f"{shellLine}.2", f"{shellLine}.0 lineend").strip()
            shellLine += 1
            shell.insert(END, "> ")
            run_command(command)
            if len(command_history) == 0 or command != command_history[-1]:
                if len(command) > 0:
                    command_history.append(command)
            history_index = 0
            if shellLine > 45:
                run_command("clear")
        # prevents backspacing beyond current line
        elif event.keysym == "BackSpace":
            prevChar = shell.get("insert - 1 chars", "insert")
            if prevChar == ">":
                shell.insert("insert", " ")
        # prevents going back to previous lines with LEFT arrow
        elif event.keysym == "Left":
            prevChar = shell.get("insert - 1 chars", "insert")
            if prevChar == ">":
                shell.mark_set("insert", "insert + 1 char")
        # prevents going back to previous lines with UP arrow also enables command history
        elif event.keysym == "Up":
            shell.mark_set("insert", f"{shellLine}.0 lineend")
            if len(command_history) - 1 - history_index >= 0:
                shell.delete(f"{shellLine}.2", END)
                prev_command = command_history[-1 - history_index]
                shell.insert(f"{shellLine}.2", prev_command)
                if len(command_history) - 1 -history_index > 0:
                    history_index += 1
        # allows you to scroll back down through command history
        elif event.keysym == "Down":
            shell.mark_set("insert", f"{shellLine}.0 lineend")
            if history_index == 0:
                shell.delete(f"{shellLine}.2", END)
            elif history_index > 0:
                history_index -= 1
                if history_index == len(command_history) - 1:
                    history_index -= 1
                shell.delete(f"{shellLine}.2", END)
                next_command = command_history[-1 - history_index]
                shell.insert(f"{shellLine}.2", next_command)
        else:
            shell.mark_set("insert", END)
    else:
        if event.char == "v":
            fig.set_visible(not fig.get_visible())
            canvas.draw()

window.bind("<Key>", keypress)
window.mainloop()