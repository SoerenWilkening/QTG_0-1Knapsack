from dataclasses import dataclass
from typing import List, Optional


@dataclass
class KnapsackSolution:
    item_assignments: Optional[List[bool]]
    objective_value: float
    elapsed_time: float
    best_bound: float
    optimal: bool
    node_count: float = 0

