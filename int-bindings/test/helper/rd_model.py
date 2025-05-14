import numpy as np


class ReadDisturbGoldenModel():

    def __init__(self, V_read, t_read):
        self.t0 = 1.55e-8
        self.fitting_parameter = 1.43339
        self.c1 = 0.0068
        self.alpha = 0.11
        self.k_B = 1.38064852e-23
        self.T = 300
        self.k = 0.003
        self.m = 0.41
        self.kb_T = self.k_B * self.T / 1.602176634e-19
        self.V_read = V_read
        self.t_read = t_read
        self.exp_tt = 1 / (self.c1 * np.exp(self.alpha * abs(self.V_read) / self.kb_T))
        self.p = self.c1 * np.exp(self.alpha * abs(self.V_read) / self.kb_T)

    def transition_time(self, N_cycle):
        return self.t0 * self.fitting_parameter**self.exp_tt * (
            1 - self.k * N_cycle**self.m)**self.exp_tt

    def G_scaling(self, t_stress, N_cycle):
        tau = self.transition_time(N_cycle)
        if (t_stress < tau):
            return 1
        else:
            return (t_stress / tau)**(-self.p)
