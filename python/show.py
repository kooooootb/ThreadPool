from config import *

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def main():
    records = []
    for time in times.keys():
        records.extend([{"time": time, "value": value} for value in times[time]])

    df = pd.DataFrame(records)
    gr = df.groupby("time")

    mean = gr.mean()
    errors = gr.std()

    mean.plot(
        yerr=errors, style='o-', legend=False, grid=True,
        xlabel='Размер пула потоков', ylabel='Среднее время обработки заявки'
    )
    plt.show()


if __name__ == '__main__':
    main()

