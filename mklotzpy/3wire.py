import math

RPD = 2 * math.pi / 360

def run():
    # get commandline args and run function
    print("3 WIRE THREAD MEASUREMENTS (60 degree threads)")

    mpi: float = 25.4

    met: bool = (input("(M)etric or [I]mperial threads?").lower() == "m")

    # thread angle
    ta: float = 60.0
    a: float = 0.5 * ta

    if met:
        tpi = float(input("Thread pitch [1 mm] ? ") or 1)
        p = tpi/mpi
    else:
        tpi = float(input("Thread pitch [20 tpi] ? ") or 20)
        p = 1.0/tpi

    print("Most common threads are single start.")

    s = float(input("Number of starts [1] ? ") or 1)

    t = 0.5 * p # width of thread in axial plane at diameter e
    w_default = 0.5 * p / math.cos(a*RPD) # wire diameter

    print(f"Best wire diameter to use = {w_default*mpi:.4F} mm = {w_default:.4F} in")

    if met:
        w = float(input(f"Wire diameter used [{w_default*mpi:.4F} mm] ? ") or w_default*mpi)
        w = w / mpi
    else:
        w = float(input(f"Wire diameter used [{w_default:.4F} in] ? ") or w_default)

    z = (3/16) * p / math.tan(a*RPD) # distance of pitch line below p/8 flat

    if not met:
        print(f"For American National Thread form, subtract {2*z:.4F} in from major diameter (assumes p/8 flat on crest) to obtain pitch diameter.")

    if met:
        md = float(input("Major diameter of thread [6 mm] ? ") or 6)
    else:
        md = float(input(("Major diameter of thread [0.25 in] ? ")) or 0.25)

    print("pd = pitch diameter, mow = measurement over wires")
    op = input("Calculate pd from mow (p) or mow from pd [m]? ")
    pd = 1 if (op == 'p') else 0

    ed = md - (2 * z) # pitch diameter

    e = ed
    b = p * s / (math.pi * e) # tangent of lead angle at pitch diameter
    q = math.tan(a*RPD)
    x = -t / q + w * (1 + 1/math.sin(a*RPD) + 0.5 * b * b * math.cos(a*RPD)/q)
    # m=e+x;

    if not pd:
        if met:
            e = float(input(f"Pitch diameter of thread [{ed*mpi:.4F} mm] ? ") or ed*mpi)
            e = e / mpi
        else:
            e = float(input(f"Pitch diameter of thread [{ed:.4F} in] ? ") or ed)
    else: 
        if met:
            m = float(input(f"Measurement over wires [{(e+x)*mpi:.4F} mm] ? ")or (e+x)*mpi)
            m = m / mpi
        else:
            m = float(input(f"Measurement over wires [{e+x:.4F} in] ? ") or e+x)

    b = p * s/(math.pi * e) # tangent of lead angle at pitch diameter
    q = math.tan(a*RPD)
    x = -t/q + w * (1 + 1/math.sin(a*RPD) + 0.5 * b * b * math.cos(a*RPD)/q)
    #  m=e+x;

    if not pd:
        print(f"Measurement over wires = {(e+x)*mpi:.4F} mm = {e+x:.4F} in")
    else:
        print(f"Pitch diameter = {(m-x)*mpi:.4F} mm = {m-x:.4F} in")

    input("Press enter to close.")

if __name__ == "__main__":
    run()
