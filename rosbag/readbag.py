"""
TO JEST PLIK TO ROZPAKOWANIA POMIAROW Z ROSBAGA
dokumentacja: https://jmscslgroup.github.io/bagpy/bagpy_example.html

ABY WYWOLYWANIE BIBLIOTEKI DZIALALO NALEZY:
    $ pip install bagpy
    $ pip install --upgrade numpy
    $ pip install --upgrade scipy


PRZED URUCHOMIENIEM SKRYPTU
1. Należy uruchomic glowne wezly przy uzyciu stero_mobile.launch
2. W nowym terminalu przejsc do folderu /rosbag
3. Uruchomic komende:
    $ rosbag record /mobile_base_controller/odom /key_vel /gazebo/model_states
4. Ta komenda powoduje ze rosbag podpina sie pod te tematy i zaczyna zczytywac
    wszystkie dane jakie zostaly tam opublikowane
5. Plik do ktorego sie te dane zapisza bedzie sie nazywal <data,godzina,itd>.bag
    i bedzie on w tym samym folderu w ktorym ten skrypt
6. Nazwe pliku polecam zmienic na jakis bardziej przyjazny, w stylu pomiar1.bag
    wtedy nie trzeba zmieniac argumentu funkcji bagreader
"""


import bagpy
from bagpy import bagreader
import pandas as pd

b = bagreader('/home/adam/tiago_ws/src/stero_mobile/rosbag/pomiary1.bag')

print(b.topic_table)

SETVAL_MSG = b.message_by_topic('/key_vel')
ODO_MSG = b.message_by_topic('/mobile_base_controller/odom')

decodedSETVAL = pd.read_csv(SETVAL_MSG)
print(decodedSETVAL)


decodedODO = pd.read_csv(ODO_MSG)
print(decodedODO)