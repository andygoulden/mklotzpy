def mandrel():
    wire_dicts = [{'type': 'music wire', 'constant': 0.980364, 'first_order': -0.012436},
                  {'type': 'phosphorus bronze', 'constant': 0.012436, 'first_order': -0.11018}]

    print("Kozo Hiraoka's SPRING WINDING MANDREL DIAMETER CALCULATION\n")
    wire_type = int(input("Wire type: music wire [0] or phosphorus bronze [1]") or "0")
    if 1 != wire_type:
        wire_type = 0
    wire_diameter = float(input("Wire diameter(inches)") or "0.040")
    internal_diameter = float(input("Spring inside diameter(inches)") or "0.203")
    spring_average_diameter = internal_diameter + wire_diameter
    empirical_factor = (wire_dicts[wire_type]['constant'] +
                        wire_dicts[wire_type]['first_order'] *
                        spring_average_diameter / wire_diameter)
    mandrel_diameter = empirical_factor * spring_average_diameter - wire_diameter
    print("\nRecommended mandrel diameter = %.3lf in\n" % mandrel_diameter)


mandrel()
