import numpy as np

numbers = np.random.randint(1, 5000000, size=100000)
# numbers = list(reversed(sorted(numbers)))
# numbers = sorted(numbers))



np.savetxt("numbers.txt", numbers, fmt="%d")
