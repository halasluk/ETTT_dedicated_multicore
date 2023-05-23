import sys
import matplotlib.pyplot as plt
import numpy as np

def generate_tasks(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string):
    figure, axis = plt.subplots(3, 3, figsize=(figs_x, figs_y))
    figure.subplots_adjust(wspace=adj_wspace, hspace=adj_hspace, top=adj_top, bottom=adj_bot, right=adj_r, left=adj_l)
    for i in range(0,9):
        num_et_tasks, num_et_jobs, is_schedulable, duration = get_data_from_file(directory, str(i))

        num_et_tasks_schedulable = num_et_tasks[np.where(is_schedulable == 1)]
        num_et_tasks_unschedulable = num_et_tasks[np.where(is_schedulable == 0)]
        num_et_tasks_timeout = num_et_tasks[np.where(is_schedulable == -1)]
        num_et_tasks_memout = num_et_tasks[np.where(is_schedulable == -2)]
        num_et_jobs_schedulable = num_et_jobs[np.where(is_schedulable == 1)]
        num_et_jobs_unschedulable = num_et_jobs[np.where(is_schedulable == 0)]
        num_et_jobs_timeout = num_et_jobs[np.where(is_schedulable == -1)]
        num_et_jobs_memout = num_et_jobs[np.where(is_schedulable == -2)]
        duration_schedulable = duration[np.where(is_schedulable == 1)]
        duration_unschedulable = duration[np.where(is_schedulable == 0)]
        duration_timeout = duration[np.where(is_schedulable == -1)]
        duration_memout = duration[np.where(is_schedulable == -2)]

        handles_tasks = []
        if len(num_et_tasks_schedulable) != 0:
            schedulable, = axis[i // 3, i % 3].plot(num_et_tasks_schedulable, duration_schedulable, 'go', ms=markersize, mew=markeredgewidth, mfc='none', label="Schedulable")
            handles_tasks.append(schedulable)
        if len(num_et_tasks_unschedulable) != 0:
            unschedulable, = axis[i // 3, i % 3].plot(num_et_tasks_unschedulable, duration_unschedulable, 'bx', ms=markersize, mew=markeredgewidth, label="Unschedulable")
            handles_tasks.append(unschedulable)
        if len(num_et_tasks_timeout) != 0:
            timeout, = axis[i // 3, i % 3].plot(num_et_tasks_timeout, duration_timeout, 'rd', ms=markersize, mew=markeredgewidth, mfc='none', label="Timeout")
            handles_tasks.append(timeout)
        if len(num_et_tasks_memout) != 0:
            memout, = axis[i // 3, i % 3].plot(num_et_tasks_memout, duration_memout, linestyle='None', color='orange', marker='2', ms=markersize, mew=markeredgewidth, label="Memout")
            handles_tasks.append(memout)

        axis[i // 3, i % 3].set_yscale("log")
        lower_lim = 10 ** (-1) if string == "TT" else 10 ** (-2)
        axis[i // 3, i % 3].set_ylim([lower_lim, (10 ** 6)*1.5])
        axis[i // 3, i % 3].set_xticks(np.arange(0, 21, step=2))
        axis[i // 3, i % 3].grid(visible=True, c="gainsboro", linewidth=1, linestyle="--")
        axis[i // 3, i % 3].set_xlabel("Number of " + string + " tasks")
        axis[i // 3, i % 3].set_ylabel("Time [ms]")
        # TRICK:
        if i == 8:
            figure.legend(handles=handles_tasks, loc="upper center", ncol=len(handles_tasks))

    plt.show()

def generate_jobs(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string):
    figure, axis = plt.subplots(3, 3, figsize=(figs_x, figs_y))
    figure.subplots_adjust(wspace=adj_wspace, hspace=adj_hspace, top=adj_top, bottom=adj_bot, right=adj_r, left=adj_l)
    for i in range(0, 9):
        num_et_tasks, num_et_jobs, is_schedulable, duration = get_data_from_file(directory, str(i))

        num_et_tasks_schedulable = num_et_tasks[np.where(is_schedulable == 1)]
        num_et_tasks_unschedulable = num_et_tasks[np.where(is_schedulable == 0)]
        num_et_tasks_timeout = num_et_tasks[np.where(is_schedulable == -1)]
        num_et_tasks_memout = num_et_tasks[np.where(is_schedulable == -2)]
        num_et_jobs_schedulable = num_et_jobs[np.where(is_schedulable == 1)]
        num_et_jobs_unschedulable = num_et_jobs[np.where(is_schedulable == 0)]
        num_et_jobs_timeout = num_et_jobs[np.where(is_schedulable == -1)]
        num_et_jobs_memout = num_et_jobs[np.where(is_schedulable == -2)]
        duration_schedulable = duration[np.where(is_schedulable == 1)]
        duration_unschedulable = duration[np.where(is_schedulable == 0)]
        duration_timeout = duration[np.where(is_schedulable == -1)]
        duration_memout = duration[np.where(is_schedulable == -2)]

        handles_jobs = []
        if len(num_et_jobs_schedulable) != 0:
            schedulable, = axis[i // 3, i % 3].plot(num_et_jobs_schedulable, duration_schedulable, 'go', ms=markersize, mew=markeredgewidth, mfc='none', label="Schedulable")
            handles_jobs.append(schedulable)
        if len(num_et_jobs_unschedulable) != 0:
            unschedulable, = axis[i // 3, i % 3].plot(num_et_jobs_unschedulable, duration_unschedulable, 'bx', ms=markersize, mew=markeredgewidth, label="Unschedulable")
            handles_jobs.append(unschedulable)
        if len(num_et_jobs_timeout) != 0:
            timeout, = axis[i // 3, i % 3].plot(num_et_jobs_timeout, duration_timeout, 'rd', ms=markersize, mew=markeredgewidth, mfc='none', label="Timeout")
            handles_jobs.append(timeout)
        if len(num_et_jobs_memout) != 0:
            memout, = axis[i // 3, i % 3].plot(num_et_jobs_memout, duration_memout, linestyle='None', color='orange', marker='2', ms=markersize, mew=markeredgewidth, label="Memout")
            handles_jobs.append(memout)

        axis[i // 3, i % 3].set_yscale("log")
        lower_lim = 10 ** (-1) if string == "TT" else 10 ** (-2)
        axis[i // 3, i % 3].set_ylim([lower_lim, (10 ** 6) * 1.5])
        axis[i // 3, i % 3].grid(visible=True, c="gainsboro", linewidth=1, linestyle="--")
        axis[i // 3, i % 3].set_xlabel("Number of " + string + " jobs")
        axis[i // 3, i % 3].set_ylabel("Time [ms]")
        # TRICK:
        if i == 8:
            figure.legend(handles=handles_jobs, loc="upper center", ncol=len(handles_jobs))

    plt.show()

def get_data_from_file(directory, filename):
    file = open(directory + filename + ".dat", "r")
    num_lines = int(file.readline().split()[0])
    num_et_tasks = np.zeros(num_lines, dtype=int)
    num_et_jobs = np.zeros(num_lines, dtype=int)
    is_schedulable = np.zeros(num_lines)
    duration = np.zeros(num_lines)
    for i in range(num_lines):
        line = file.readline().split()
        num_et_tasks[i] = line[0]
        num_et_jobs[i] = line[1]
        is_schedulable[i] = line[2]
        duration[i] = line[3]
    file.close()
    return num_et_tasks, num_et_jobs, is_schedulable, duration

def main():

    ## SMALL PLOTS
    markersize = 4
    markeredgewidth = 0.75
    figs_x = 10
    figs_y = 10
    adj_wspace = 0.3
    adj_hspace = 0.35
    adj_top = 0.93
    adj_bot = 0.09
    adj_r = 0.99
    adj_l = 0.07

    directory = sys.argv[1]
    string = "ET"
    generate_tasks(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string)
    generate_jobs(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string)
    directory = sys.argv[2]
    string = "TT"
    generate_tasks(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string)
    generate_jobs(directory, markersize, markeredgewidth, figs_x, figs_y, adj_wspace, adj_hspace, adj_top, adj_bot, adj_r, adj_l, string)

if __name__ == "__main__":
    main()