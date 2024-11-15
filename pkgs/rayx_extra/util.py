import itertools
import functools
import copy

def merge(a, b):
    merged = {}
    if isinstance(a, dict) and isinstance(b, dict):
        for key in a.keys() | b.keys():
            if key in a and key in b:
                merged[key] = merge(a[key], b[key])
            elif key in a:
                merged[key] = a[key]
            else:
                merged[key] = b[key]
    else:
        raise ValueError(f"Cannot merge {a} and {b}")
    return merged

def to_nested(params):
    nested = {}
    for key, value in params.items():
        parts = key.split('.')
        temp = {parts[-1]: value}
        for part in reversed(parts[:-1]):
            temp = {part: temp}
        nested = merge(nested, temp)
    return nested

def _normalize(params):
    if isinstance(params, list):
        result = []
        for item in params:
            result.extend(_normalize(item))
        return result
    elif isinstance(params, dict):
        result = []
        for key, value in params.items():
            normalized = _normalize(value)
            if len(result) == 0:
                result = [{key: v} for v in normalized]
            elif len(result) == len(normalized):
                for i in range(len(result)):
                    result[i] = merge(result[i], {key: normalized[i]})
            else:
                raise ValueError(f"unequal lengths: {len(result)} != {len(normalized)}")
        return result
    else:
        return [params]

def normalize(params):
    return list(map(to_nested, _normalize(params)))

def all_combinations(*args):
    args = list(map(normalize, args))
    product = list(itertools.product(*args))
    merged = map(lambda ntuple: functools.reduce(merge, ntuple, {}), product)
    return list(merged)

def trace_n(beamline, params):
    orig_sources = copy.deepcopy(beamline.sources)
    orig_elements = copy.deepcopy(beamline.elements)
    orig = {
        **orig_sources,
        **orig_elements
    }

    def change_params(obj, params):
        for key, value in params.items():
            if isinstance(value, dict):
                change_params(getattr(obj, key), value)
            else:
                setattr(obj, key, value)

    def reset_params(obj, orig, params):
        for key, value in params.items():
            if isinstance(value, dict):
                reset_params(getattr(obj, key), getattr(orig, key), value)
            else:
                setattr(obj, key, getattr(orig, key))
    ps = []
    results = []
    normalized = normalize(params)
    for p in normalized:
        print(f"Running with parameters: {p}")
        for elem, params in p.items():
            change_params(beamline[elem], params)
        df = beamline.trace()
        ps.append(p)
        results.append(df)
        for elem, params in p.items():
            reset_params(beamline[elem], orig[elem], params)
    return ps, results

