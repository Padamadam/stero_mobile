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
# b = bagreader('/home/adam/tiago_ws/src/stero_mobile/rosbag/r_s5_v1_a05.bag')
b = bagreader('/home/lnowaczy/tiago_ws/src/stero_mobile/rosbag/l_s5_v1_a1.bag')
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

set_angularz_data = hSETVAL['angular.z'].tolist()

real_linearx_data = hODO['twist.twist.linear.x'].tolist()
real_linearx_time = hODO['Time'].tolist()

real_angularz_data = hODO['twist.twist.angular.z'].tolist()

# Zzipuj dane z czasem ich odczytu aby znac moment odczytania rekordu
zipped_real_linear = list(zip(real_linearx_data, real_linearx_time))
zipped_set_linear = list(zip(set_linearx_data, set_linearx_time))

zipped_real_angular = list(zip(real_angularz_data, real_linearx_time))
zipped_set_angular = list(zip(set_angularz_data, set_linearx_time))

# teraz niezle jajca, bo ogolnie to dwa topici publikuja dane z rozna czestotliwoscia
# wiec aby wgl moc przyrownac do siebie rekordy predkosci zadanej i faktycznej
# to trzeba uzgodnic ze porownujemy je z tej samej chwili odczytu
# a problem jest taki ze tych danych z predkosci faktycznej jest po prostu wiecej
real_linearx_analysis = []
set_linearx_analysis = []
total_linearx_error = 0
linearx_error_table = []
state = 0

#ruch jednostajny
const_vel_error = []
total_const_vel_error = 0

#ruch opozniony
delay_vel_error =[]
total_delay_vel_error = 0

set_delay_vel_analysis = []
real_delay_vel_analysis = []

constvel = 1.0

state = 0

bok = 1
measurement = 4

i = 1

# przeiteruj po wszystkich rekordach
for xreal, xset in itertools.product(zipped_real_linear, zipped_set_linear):
    # jesli w przyblizeniu maja one ten sam czas odczytu
    # to przepisz je do listy tak aby dane w niej odpowiadaly sobie
    # wedlug chwili odczytu

    if round(xreal[1], 1) == round(xset[1], 1):
        # te rekordy byly pobrane w tej samej chwili
        # okresl czy jest to stan 1 powinno jeszcze sprawdzac predkosc katowa ze jest 0
        # albo czy juz jestesmy w stanie sprawdzania stanu 1

        if state == 0 and bok <= measurement:
            # stan rozpoznajacy poczatek cyklu
            if (round(xset[0], 2) == 0 and round(xreal[0], 2) == 0):
                state = 1

        if  state == 1: # or (xset[0] == 0 and round(xreal[0], 2) == 0):
            
            # stan 1 - przyspieszanie

            linearx_error = abs(xreal[0] - xset[0])
            linearx_error_table.append(linearx_error)
            total_linearx_error += linearx_error
        
            set_linearx_analysis.append(xset[0])
            real_linearx_analysis.append(xreal[0])
            # print(f'{round(xset[0], 2)}, {round(xreal[0], 2)}')
            # sprawdz czy juz dojechal do predkosci koncowej
            if (round(xset[0], 2) == constvel and round(xreal[0], 2) == constvel): #or state == 2:
                #jazda v_const
                state = 2
        if state == 2:
        # stan 2 - ruch jednostajny prostoliniowy
            momentary_const_vel_error = abs(xreal[0] - xset[0])
            const_vel_error.append(momentary_const_vel_error)
            total_const_vel_error += momentary_const_vel_error

            if (round(xset[0], 2) != constvel and round(xreal[0], 2) == constvel):
                state = 3
        if state == 3:
        # stan 3 - ruch opozniony
        
            momentary_delay_vel_error = abs(xreal[0] - xset[0])
            delay_vel_error.append(momentary_delay_vel_error)
            total_delay_vel_error += momentary_delay_vel_error
            set_delay_vel_analysis.append(xset[0])
            real_delay_vel_analysis.append(xreal[0])

            if (round(xset[0], 2) == 0 and round(xreal[0], 2) == 0):
                state = 4
        if state == 4:
            # break wywala z petli po pierwszym boku kwadratu
            # break
            # state 0 powtarza petle 4 razy
            state = 0
            bok += 1

state=0
bok = 1
angularz_error_table=[]
total_angularz_error=0
set_angularz_analysis=[]
real_angularz_analysis=[]
total_of_totals_error_angular = 0

for zreal, zset in itertools.product(zipped_real_angular, zipped_set_angular):
    if round(zreal[1], 1) == round(zset[1], 1):
        if state == 0 and bok <= measurement:
        # stan rozpoznajacy poczatek cyklu obrotu
            if (zset[0] != 0 and round(zreal[0], 2) != 0):
                state = 1
        if state == 1:
            momentary_angularz_error = abs(zreal[0]-zset[0])
            angularz_error_table.append(momentary_angularz_error)
            total_angularz_error += momentary_angularz_error

            set_angularz_analysis.append(zset[0])
            real_angularz_analysis.append(zreal[0])
            if (zset[0] == 0 and round(zreal[0], 2) == 0):
                # po skonczonym obrocie dodajemy blad z calego obrotu do zmiennej
                # bledu po 4 obrotach
                total_of_totals_error_angular += total_angularz_error
                state = 0
                bok += 1
                

# idk label nie dziala :ccc

# Wykresy ruchu przyspieszonego
plt.figure(1)
plt.plot(real_linearx_analysis, label='real')
plt.plot(set_linearx_analysis, label = 'set')
plt.title('Trajektoria ruchu przyspieszonego')

plt.figure(2)
plt.plot(linearx_error_table)
plt.title('Blad ruchu przyspieszonego')

# Wykresy ruchu jednostajnego
plt.figure(3)
plt.plot(const_vel_error)
plt.title('Blad ruchu jednostajnego')

# Wykresy ruchu opoznionego
plt.figure(4)
plt.plot(real_delay_vel_analysis, label='real')
plt.plot(set_delay_vel_analysis, label = 'set')
plt.title('Trajektoria ruchu opoznionego')

plt.figure(5)
plt.plot(delay_vel_error)
plt.title('Blad ruchu opoznionego')

# Wykresy predkosci katowej
plt.figure(6)
plt.plot(real_angularz_analysis, label='real')
plt.plot(set_angularz_analysis, label = 'set')
plt.title('Trajektoria ruchu obrotowego')

plt.figure(7)
plt.plot(angularz_error_table)
plt.title('Blad ruchu obotowego')
plt.show()

# dane potrzebne do sprawka

# całkowita suma błedow (liniowych) - z jednej prostej
# jezeli w pierwszej petli w ostatnim if dasz state = 0 zamiast brake bedzie to blad po calym kwadracie


total_of_totals_error = total_linearx_error + total_const_vel_error + total_delay_vel_error
print(f'suma bledow liniowych to {total_of_totals_error}')

# calkowita suma bledow (katowych) - z calego kwadratu 
total_of_totals_error_angular
print(f'suma bledow katowych to {total_of_totals_error_angular}')

print(f"""
opniziony = {total_delay_vel_error}
przyspieszony = {total_linearx_error}
jednostajny = {total_const_vel_error}
Numer boku: {measurement}
blad liniowy: {total_of_totals_error}
blad katowy: {total_of_totals_error_angular}
""")
