"""
TO JEST PLIK DO ROZPAKOWANIA POMIAROW Z ROSBAGA
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


import itertools
import bagpy
from bagpy import bagreader
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Odczytaj plik do ktorego spakowano pomiary rosbagiem
b = bagreader('/home/adam/tiago_ws/src/stero_mobile/rosbag/r_s5_v1_a05.bag')

# Wyprintuj tabele topicow aby dowiedziec sie jakie sie zapisaly i ile maja recordow
print(b.topic_table)

# Stworz handler pliku CSV dla topicu
SETVAL_MSG = b.message_by_topic('/key_vel')
ODO_MSG = b.message_by_topic('/mobile_base_controller/odom')
# POSE_MSG = b.message_by_topic('/gazebo/model_states')

# Stworz handler tablicy CSV dla handlera pliku
hSETVAL = pd.read_csv(SETVAL_MSG)
hODO = pd.read_csv(ODO_MSG)
# hPOSE = pd.read_csv(POSE_MSG)


# Odczytaj kolumne danych i przekonwertuj ja do listy
set_linearx_data = hSETVAL['linear.x'].tolist()
set_linearx_time = hSETVAL['Time'].tolist()

set_angularz_data = hSETVAL['angular.x'].tolist()

real_linearx_data = hODO['twist.twist.linear.x'].tolist()
real_linearx_time = hODO['Time'].tolist()

# Zzipuj dane z czasem ich odczytu aby znac moment odczytania rekordu
zipped_real_linear = list(zip(real_linearx_data, real_linearx_time))
zipped_set_linear = list(zip(set_linearx_data, set_linearx_time))


# teraz niezle jajca, bo ogolnie to dwa topici publikuja dane z rozna czestotliwoscia
# wiec aby wgl moc przyrownac do siebie rekordy predkosci zadanej i faktycznej
# to trzeba uzgodnic ze porownujemy je z tej samej chwili odczytu
# a problem jest taki ze tych danych z predkosci faktycznej jest po prostu wiecej
real_linearx_analysis = []
set_linearx_analysis = []
total_linearx_error = 0
linearx_error_table = []
state = 0

constvel = 1.0

# przeiteruj po wszystkich rekordach
for xreal, xset in itertools.product(zipped_real_linear, zipped_set_linear):
    # jesli w przyblizeniu maja one ten sam czas odczytu
    # to przepisz je do listy tak aby dane w niej odpowiadaly sobie
    # wedlug chwili odczytu
    if round(xreal[1], 1) == round(xset[1], 1):
        # te rekordy byly pobrane w tej samej chwili

        # okresl czy jest to stan 1 powinno jeszcze sprawdzac predkosc katowa ze jest 0
        # albo czy juz jestesmy w stanie sprawdzania stanu 1
        if (xset[0] == 0 and round(xreal[0], 2) == 0) or state == 1:
            # stan 1 - przyspieszanie
            state = 1

            linearx_error = abs(xreal[0] - xset[0])
            linearx_error_table.append(linearx_error)
            total_linearx_error += linearx_error
        
            set_linearx_analysis.append(xset[0])
            real_linearx_analysis.append(xreal[0])

            if (xset[0] == constvel and round(xreal[0], 2) == constvel):
                state = 2
                break



# idk label nie dziala :ccc
plt.figure(1)
plt.plot(real_linearx_analysis, label='real')
plt.plot(set_linearx_analysis, label = 'set')

plt.figure(2)
plt.plot(linearx_error_table)
plt.show()






