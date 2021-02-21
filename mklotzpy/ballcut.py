import math

RPD = 2 * math.pi / 360

def wdata(ofile, d, ds, fdx, dtheta, r, rs, xinc):

    with open(ofile,"w") as fp:
        fp.write("Incremental Sphere Turning Data\n")
        fp.write(f"Sphere diameter = {d:.4F} in\n")
        fp.write(f"Stock diameter = {ds:.4F} in\n")

        if not fdx:
            fp.write(f"Angular increment = {dtheta:.4F} deg\n\n")
        else:
            fp.write(f"X increment = {xinc:.4F} in\n\n")

        fp.write(f"N = cut number\n")
        fp.write(f"XF = axial (along lathe bed) position of tool\n")
        fp.write(f"DX = increment in x from last cut\n")
        fp.write(f"YF = depth of cut\n")
        fp.write(f"DY = increment in y from last cut\n")
        fp.write(f"WD = work diameter resulting from depth of cut YF\n\n")
        fp.write(f"  N       XF       DX       YF       DY       WD\n\n")

        theta = 0
        nc = 0
        dx = 0
        dy = 0
        dcl = 0
        fl = 0
        x = 0
        xl = 0

        if not fdx:

            while theta <= 90:
                x = r * math.cos(theta*RPD)
                y = r * math.sin(theta*RPD)
                dc= rs - y
                f = r - x
                dw = 2 * y
                if dc < 0:
                    break
                if nc:
                    dx = f - fl
                    dy = dc - dcl
                fp.write(f"{nc:3d}{f:9.3F}{dx:+9.3F}{dc:9.3F}{dy:+9.3F}{dw:9.3F}\n")
                fl = f
                dcl = dc

                theta += dtheta
                nc += 1
        else:
            while x <= r:
                y = math.sqrt(r * r - (x - r) * (x - r))
                dc = rs - y
                dw = 2 * y
                if dc < 0: 
                    break
                if nc: 
                    dx = x - xl
                    dy = dc - dcl
                fp.write(f"{nc:3d}{x:9.3F}{dx:+9.3F}{dc:9.3F}{dy:+9.3F}{dw:9.3F}\n")
                xl = x
                dcl = dc

                x += xinc
                nc += 1


def run():
    print("INCREMENTAL SPHERE SHAPING ON LATHE")

    d = float(input("Sphere diameter [1 in] ? ") or 1)
    ds = float(input(f"Stock diameter [{d} in] ? ") or d)

    op = input("Constant [A]ngle step or constant (X) step ? ")
    fdx = 1 if (op.lower() == 'x') else 0

    dtheta = 0
    xinc = 0

    if not fdx:
        dtheta = float(input("Angular increment [5 deg] ? ") or 5)
    else:
        xinc = float(input("X increment [0.02 in] ? ") or 0.02)
        
    r = 0.5 * d
    rs = 0.5 * ds

    ofile = "BALLCUT.OUT"

    wdata(ofile, d, ds, fdx, dtheta, r, rs, xinc)

    print(F"\nYour data is on: {ofile}")


if __name__ == "__main__":
    run()
