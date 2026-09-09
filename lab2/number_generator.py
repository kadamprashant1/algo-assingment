import numpy as np

numbers = np.random.randint(1, 1000000, size=100000)
numbers = list(reversed(sorted(numbers)))

np.savetxt("numbers_sorted_reversed.txt", numbers, fmt="%d")
