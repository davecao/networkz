# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.spatial.distance import cdist
import networkx as nx


# %%
file="example.csv"
df = pd.read_csv(file, header=0)
display(df)


# %%
d = cdist(df["val"].values.reshape(-1, 1), df["val"].values.reshape(-1, 1), 'cityblock')
dist_df = pd.DataFrame(d, columns=df.index, index=df.index)
display(dist_df)


# %%
### Create a graph
G = nx.from_numpy_matrix(dist_df.values)


# %%
print("There are {} nodes and {} edges.".format(G.number_of_nodes(), G.number_of_edges()))


# %%
pos = nx.spring_layout(G, k=0.3)


# %%
nx.draw(G, with_labels=True, node_color='skyblue', edge_cmap=plt.cm.Blues, pos = pos)
for e in G.edges():
    w = dist_df.loc[e[0], e[1]]
    G[e[0]][e[1]]["weight"] = w
edges = [e for e in G.edges()]
edge_labels = nx.get_edge_attributes(G,'weight') # key is edge, pls check for your case
formatted_edge_labels = {(elem[0],elem[1]):edge_labels[elem] for elem in edge_labels} # use this to modify the tuple keyed dict if it has > 2 elements, else ignore
nx.draw_networkx_edge_labels(G,pos,edge_labels=formatted_edge_labels,font_color='red')
plt.show()


# %%



