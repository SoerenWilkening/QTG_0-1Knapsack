from .bindings import Knapsack
import os


def save_instance(instance: Knapsack, out_dir: str):
    os.makedirs(out_dir, exist_ok=True)
    with open(os.path.join(out_dir, f"{instance.name}.knap"), "w") as f:
        f.write(f"{instance.size}\n")
        for item in instance.items:
            f.write(f"{item.profit} {item.cost}\n")
        f.write(str(instance.capacity))


def load_instance(file_path: str):
    with open(file_path, "r") as f:
        lines = f.readlines()
        size = int(lines[0])
        items = []
        for line in lines[1:-1]:
            profit, cost = line.split(" ")
            items.append((int(profit), int(cost)))
        capacity = int(lines[-1])

        return Knapsack(size, items, capacity, os.path.basename(file_path))