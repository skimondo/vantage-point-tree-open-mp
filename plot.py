import pandas as pd
import matplotlib.pyplot as plt

# Read the data from granularite.dat
data = pd.read_csv("granularite.dat", sep=" ", header=None, names=["max_depth", "execution_time"])

# Calculate the average execution time for each max_depth
avg_data = data.groupby("max_depth")["execution_time"].mean().reset_index()

# Plotting the data
plt.figure(figsize=(10, 6))
plt.plot(avg_data["max_depth"], avg_data["execution_time"], marker='o', linestyle='-')
plt.xlabel("Max Depth")
plt.ylabel("Average Execution Time (s)")
plt.title("Average Execution Time vs Max Depth")
plt.grid(True)
plt.savefig("average_execution_time_vs_max_depth.png")  # Save the plot as an image file
plt.close()


# Read the data from weak_scale.dat
data = pd.read_csv("weak_scale.dat", sep=" ", header=None, names=["core_count", "node_count", "execution_time", "throughput"])

# Calculate the average throughput for each core_count
avg_data = data.groupby("core_count")["throughput"].mean().reset_index()

# Plotting core_count vs. average throughput
plt.figure(figsize=(10, 6))
plt.plot(avg_data["core_count"], avg_data["throughput"], marker='o', linestyle='-')
plt.xlabel("Core Count")
plt.ylabel("Average Throughput (Million Nodes per Second)")
plt.title("Average Throughput vs Core Count")
plt.grid(True)
plt.savefig("average_throughput_vs_core_count.png")  # Save the plot as an image file
plt.close()
