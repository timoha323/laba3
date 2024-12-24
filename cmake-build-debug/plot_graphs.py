import matplotlib.pyplot as plt
import os

def plot_file(filename):
    times = []
    elements = []
    with open(filename, 'r') as f:
        for line in f:
            time, num_elements = map(float, line.split())
            times.append(time)
            elements.append(num_elements)
    plt.plot(elements, times, label=filename)

def main():
    files = ["result_time_matrix_hash.txt", "result_time_matrix_b.txt", "result_time_vector_hash.txt", "result_time_vector_b.txt"]
    for file in files:
        if os.path.exists(file):
            plot_file(file)
    plt.xlabel('Number of Elements')
    plt.ylabel('Time')
    plt.legend()
    plt.title('Performance Graphs')
    plt.show()

if __name__ == '__main__':
    main()
