from fzzpy import compute, parse_filtration_file, write_persistence_intervals
import tempfile
import os

def test_persistence_computation():
    # Define the input and expected output
    input_data = [
        "i 0", "i 1", "i 2", "i 0 1", "i 0 2", "i 1 2", "i 0 1 2",
        "d 0 1 2", "d 1 2", "d 0 1"
    ]
    expected_output = [(0, 2, 3), (0, 3, 4), (1, 6, 6), (0, 1, 10), (0, 10, 10), (1, 8, 8)]

    # Create a temporary file for the input data
    with tempfile.NamedTemporaryFile(delete=False, mode='w') as tmpfile:
        tmpfile.write('\n'.join(input_data))

    # Load and compute
    filt_simp, filt_op = parse_filtration_file(tmpfile.name)
    result = compute(filt_simp, filt_op)  # Replace with the actual compute function

    # Check the result
    assert result == expected_output

    # Write the result to another temporary file
    with tempfile.NamedTemporaryFile(delete=False, mode='w') as tmpfile:
        write_persistence_intervals(result, tmpfile.name)

    # This is just to demonstrate reading back the file if needed
    with open(tmpfile.name, 'r') as f:
        written_data = f.readlines()
        written_data = [tuple(map(int, line.strip().split())) for line in written_data]
        assert written_data == expected_output

    # Delete the temporary file manually
    os.remove(tmpfile.name)

