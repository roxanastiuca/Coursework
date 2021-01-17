import argparse

# Computes the matrix delta for a given pattern 'pat'.
def compute_delta(pat):
	delta = []

	# Compute first line in delta:
	line = [0]*26
	line[ord(pat[0]) - ord('A')] = 1
	delta.append(line)

	pat_chars = list(set(pat))

	# Compute each line in the delta matrix:
	for i in range(1, len(pat)+1):
		line = [0]*26

		# Find next state for each of the characters found in
		# the pattern. For characters not found in the pattern
		# the next state is always 0.
		for ch in pat_chars:
			word = pat[:i] + ch

			# Try to find the biggest prefix to match word.
			for j in range(i+1, 0, -1):
				if pat[:j] == word[(i+1-j):]:
					line[ord(ch) - ord('A')] = j
					break

		# Append the new line to the delta matrix.
		delta.append(line)

	return delta

# Performs the Boyer-Moore algorithm and finds the list of offsets
# where pattern 'pat' can be found in the text.
def boyer_moore(pat, text):
	delta = compute_delta(pat)

	offset_list = []
	state = 0

	for i in range(0, len(text)):
		# Switch to next state.
		state = delta[state][ord(text[i]) - ord('A')]

		# If final state found, add offset to list.
		if state == len(pat):
			offset_list.append(i - len(pat) + 1)

	return offset_list

if __name__ == "__main__":
	# Get input and output file names from program arguments.
	parser = argparse.ArgumentParser(description = 'String parser.')
	parser.add_argument('input', help = 'path to input file')
	parser.add_argument('output', help = 'path to output file')
	args = parser.parse_args()

	# Read input file.
	strings = []
	with open(args.input, "r") as fin:
		strings = fin.readlines()

	if len(strings) != 2:
		print("Invalid input file format.")
		exit()

	string1 = strings[0].strip()
	string2 = strings[1].strip()

	# Perform the Boyer-Moore algorithm in order to find the offsets
	# where string1 can be found in string2.
	offset_list = boyer_moore(string1, string2)

	# Write offset list to output file.
	with open(args.output, "w") as fout:
		if len(offset_list) == 0:
			fout.write('\n')
		else:
			fout.write(' '.join(map(str, offset_list)) + ' \n')