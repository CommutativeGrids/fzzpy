def parse_filtration_file(filename):
    """
    Parse a filtration file 
    and return a list of simplices and 
    a list of booleans indicating insertion or deletion.
    """
    with open(filename, 'r') as f:
        content = f.read()
    
    lines = content.strip().split("\n")
    filt_simp = []
    filt_op = []

    for line in lines:
        parts = line.split()
        op = parts[0]
        simp = list(map(int, parts[1:]))
        
        filt_simp.append(simp)
        if op == "i":
            filt_op.append(True)
        elif op == "d":
            filt_op.append(False)

    return filt_simp, filt_op

def write_persistence_intervals(tuples, filename):
    with open(filename, 'w') as f:
        lines = [' '.join(map(str, tup)) for tup in tuples]
        f.write('\n'.join(lines))





