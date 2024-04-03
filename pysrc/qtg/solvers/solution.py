from dataclasses import dataclass
from typing import List, Optional


@dataclass
class KnapsackSolution:
    item_assignments: Optional[List[bool]]
    objective_value: float
    elapsed_time: float
    lower_bound: float
    optimal: bool

