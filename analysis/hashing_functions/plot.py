import sys
import matplotlib.pyplot as plt
import pandas as pd

if(len(sys.argv) != 2):
    exit(0)

data = pd.read_csv(sys.argv[1], sep='\t')
print(data)
plt.figure(figsize=(3, 4))
data.plot(kind='bar')
plt.savefig("plot", dpi=400)