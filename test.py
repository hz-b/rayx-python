import matplotlib.pyplot as plt
import numpy as np
import rayx as rx
import rayx_extra.util as u
combs = u.all_combinations({
        'Slit': {
            'openingWidth': [2.0, 3.0, 4.0],
            'openingHeight': [1.0, 2.0, 3.0]
        },
    }, {
        'Matrix Source': {
            'numberOfRays': [1e3, 1e4]
        }
    })
params = [
    combs,
    {
        'Slit.worldPosition.z': 1000.0,
        'Slit.distancePreceding': 1000.0
    }
]

bl = rx.open_beamline('/home/valle/DEV/rayx-py/rayx/Intern/rayx-core/tests/input/slit1_seeded.rml')

#print(bl['Matrix Source'])
#print(bl['Slit'])

ps, results = u.trace_n(bl, params)

for p, df in zip(ps, results):
    onImgPlane = df[df['lastElement'] == 'ImagePlane']
    poss = onImgPlane.loc[:, 'position'].values
    if len(poss) > 0:
        vals = np.vstack(poss)
        plt.scatter(vals[:, 0], vals[:, 2])
        plt.title(f'{p}')
        plt.show()
