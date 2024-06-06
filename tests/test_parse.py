import pytest
from pathlib import Path
from qtg.utils import load_instance


def test_parse_jooken():
    path = Path(__file__).parent / "data" / "650.knap"
    instance = load_instance(path)

    assert len(instance.items) == 650
    assert instance.capacity == 10000000000
    assert instance.items[0].profit == 5000100296
    assert instance.items[0].cost == 5000100300


def test_parse_smith_miles():
    path = Path(__file__).parent / "data" / "smith_miles.knap"
    instance = load_instance(path)
    assert len(instance.items) == 1000
    assert instance.capacity == 452512
    assert instance.items[0].cost == 302
    assert instance.items[0].profit == 394
