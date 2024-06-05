from pathlib import Path, PosixPath

from .bindings import Knapsack, Item, ItemVector
import os


def save_instance(instance: Knapsack, out_dir: str):
    os.makedirs(out_dir, exist_ok=True)
    with open(os.path.join(out_dir, f"{instance.name}.knap"), "w") as f:
        f.write(f"{instance.size}\n")
        for item in instance.items:
            f.write(f"{item.profit} {item.cost}\n")
        f.write(str(instance.capacity))


def load_smith_miles(file_path, lines):
    """
    Load a Smith-Miles instance from a list of lines.
    :param lines:
    :return:
    """
    lines = lines[2:]
    capacity = int(lines[0])
    items = ItemVector()
    for line in lines[1:]:
        cost, profit = line.split("\t")
        items.append(Item(int(profit), int(cost)))

    knapsack = Knapsack(len(items), capacity, items, os.path.basename(file_path))
    return knapsack


def load_kplib(file_path, lines):
    """
    Load a KP-Lib instance from a list of lines.
    :param lines:
    :return:
    """
    size = int(lines[1])
    capacity = int(lines[2])
    items = ItemVector()
    for line in lines[4:]:
        cost, profit = line.split()
        items.append(Item(int(profit), int(cost)))

    assert size == len(items)

    knapsack = Knapsack(size, capacity, items, os.path.basename(file_path))
    return knapsack


def load_instance(file_path: str):
    if isinstance(file_path, PosixPath) or isinstance(file_path, Path):
        file_path = str(file_path)

    with open(file_path, "r") as f:
        lines = f.readlines()

        if file_path.endswith(".kp"):
            return load_kplib(file_path, lines)

        if not lines[0].rstrip().isnumeric():
            return load_smith_miles(file_path, lines)

        size = int(lines[0])
        items = ItemVector()
        for line in lines[1:-1]:
            if len(line.split()) == 2:
                profit, cost = line.split()
            else:
                _, profit, cost = line.split()
            items.append(Item(int(profit), int(cost)))
        capacity = int(lines[-1])

        knapsack = Knapsack(size, capacity, items, os.path.basename(file_path))
        return knapsack
