import numpy as np
import pandas as pd
import sys
import tensorflow as tf
import matplotlib.pyplot as plt



def add_data(hmap, data, ts):
    if ts not in hmap:
        hmap[ts] = data
    elif data != hmap[ts]:
        print(f"WHY {ts} is already in: {hmap}")

training_files = ["../data/bicep_curl/suzan_bicep_set1.log", "../data/bicep_curl/jake_bicep_set1.log", "../data/bicep_curl/udai_bicep_set1.log", "../data/shoulder_press/suzan_shoulder_set1.log", "../data/shoulder_press/jake_shoulder_set1.log", "../data/shoulder_press/udai_shoulder_set1.log", "../data/row/suzan_row_set1.log", "../data/row/jake_row_set1.log", "../data/row/udai_row_set1.log", "../data/rdl/suzan_rdl_set1.log", "../data/rdl/jake_rdl_set1.log", "../data/rdl/jessica_rdl_set1.log", "../data/squat/suzan_squat_set1.log", "../data/squat/jake_squat_set1.log", "../data/squat/udai_squat_set1.log"]
test_files = ["../data/bicep_curl/suzan_bicep_set2.log", "../data/bicep_curl/jake_bicep_set2.log", "../data/bicep_curl/udai_bicep_set2.log", "../data/shoulder_press/suzan_shoulder_set2.log", "../data/shoulder_press/jake_shoulder_set2.log", "../data/shoulder_press/udai_shoulder_set2.log", "../data/row/suzan_row_set2.log", "../data/row/jake_row_set2.log", "../data/row/udai_row_set2.log", "../data/rdl/suzan_rdl_set2.log", "../data/rdl/jake_rdl_set2.log", "../data/rdl/jessica_rdl_set2.log", "../data/squat/suzan_squat_set2.log", "../data/squat/jake_squat_set2.log", "../data/squat/udai_squat_set2.log"]
labels = ["bicep_curl", "bicep_curl", "bicep_curl", "shoulder_press", "shoulder_press", "shoulder_press", "row", "row", "row", "rdl", "rdl", "rdl", "squat", "squat", "squat"]
y_train = np.asarray(labels, dtype="object")
y_train = pd.DataFrame(y_train)
X_train = []
X_test = []

def create_df(filename, dfs):
    print(filename)
    with open(filename, 'r') as file:
        timestamps = []
        accel_x = {}
        accel_y = {}
        accel_z = {}
        gyro_x = {}
        gyro_y = {}
        gyro_z = {}

        for line in file:
            data_line = line.strip()
            if data_line and data_line[0].isdigit():
                # print(data_line)
                _, data_type, timestamp, data = data_line.split(",")
                timestamp = int(timestamp)
                data = float(data)
                idx = None
                if timestamp not in timestamps:
                    timestamps.append(timestamp)
                match data_type:
                    case "AX":
                        add_data(accel_x, data, timestamp)
                    case "AY":
                        add_data(accel_y, data, timestamp)
                    case "AZ":
                        add_data(accel_z, data, timestamp)
                    case "GX":
                        add_data(gyro_x, data, timestamp)
                    case "GY":
                        add_data(gyro_y, data, timestamp)
                    case "GZ": 
                        add_data(gyro_z, data, timestamp)
        
        for ts in timestamps:
            if ts not in accel_x:
                accel_x[ts] = None
            if ts not in accel_y:
                accel_y[ts] = None
            if ts not in accel_z:
                accel_z[ts] = None
            if ts not in gyro_x:
                gyro_x[ts] = None
            if ts not in gyro_y:
                gyro_y[ts] = None
            if ts not in gyro_z:
                gyro_z[ts] = None


        accel_x = dict(sorted(accel_x.items()))
        accel_y = dict(sorted(accel_y.items()))
        accel_z = dict(sorted(accel_z.items()))
        gyro_x = dict(sorted(gyro_x.items()))
        gyro_y = dict(sorted(gyro_y.items()))
        gyro_z = dict(sorted(gyro_z.items()))
                    
        # print(f"timestamps: {timestamps}")
        # print(len(timestamps))
        # print(f"accel_x: {accel_x}")
        # print(len(accel_x))
        # print(f"accel_y: {accel_y}")
        # print(len(accel_y))
        # print(f"accel_z: {accel_z}")
        # print(len(accel_z))
        # print(f"gyro_x: {gyro_x}")
        # print(len(gyro_x))
        # print(f"gyro_y: {gyro_y}")
        # print(len(gyro_y))
        # print(f"gyro_z: {gyro_z}")
        # print(len(gyro_z))

        df = pd.DataFrame({'timestamp': timestamps, 'accel_x': accel_x.values(), 'accel_y': accel_y.values(), 'accel_z': accel_z.values(), 'gyro_x': gyro_x.values(), 'gyro_y': gyro_y.values(), 'gyro_z': gyro_z.values()})
        # df.to_csv('raw_df.csv')
        cleaned_df = df.interpolate(method="linear")
        name = "../csvs/" + filename.split("/")[-1].split(".")[0] + "_df.csv"
        if len(sys.argv) > 1 and sys.argv[1] == "write":
            cleaned_df.to_csv(name, index=False)
        print(cleaned_df.head())
        dfs.append(cleaned_df)
        # cleaned_df.plot(x="timestamp")
        # plt.title("Bicep Curls")
        # plt.xlabel("Timestamp")
        # plt.ylabel("IMU Data")
        # plt.show()




for filename in training_files:
    create_df(filename, X_train)

for filename in test_files:
    create_df(filename, X_test)

