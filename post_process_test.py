import numpy as np
import matplotlib.pyplot as plt


data_original = np.genfromtxt('./tests/cases/output.csv', delimiter=',', dtype=None, encoding=None)


# Define conversion factors
Pa_to_mmHg = 0.00750062  # Convert Pascal to mmHg
m3_to_mL = 1e6           # Convert cubic meters to milliliters
m3s_to_mLs = 1e6         # Convert cubic meters per second to milliliters per second


mask_p_in_original = data_original[:,0] == "pressure:IN:upstream_vessel"
p_in_original_data = data_original[mask_p_in_original]
p_in_original_data[:,2] = p_in_original_data[:,2].astype(float)*Pa_to_mmHg




mask_p_ventricle_original = data_original[:,0] == "pressure:inlet_valve:ventricle"
p_ventricle_original_data = data_original[mask_p_ventricle_original]
p_ventricle_original_data[:,2] = p_ventricle_original_data[:,2].astype(float)*Pa_to_mmHg  




mask_p_out_original = data_original[:,0] == "pressure:outlet_valve:downstream_vessel"
p_out_original_data = data_original[mask_p_out_original]
p_out_original_data[:,2] = p_out_original_data[:,2].astype(float)*Pa_to_mmHg




mask_v_original = data_original[:,0] == "volume:ventricle"
volume_data_original = data_original[mask_v_original]
volume_data_original[:,2] = volume_data_original[:,2].astype(float)*m3_to_mL




mask_out_out = data_original[:,0] == "flow:ventricle:outlet_valve"
outflow_data_original = data_original[mask_out_out]
outflow_data_original[:,2] = outflow_data_original[:,2].astype(float)*m3s_to_mLs




mask_in_original = data_original[:,0] == "flow:inlet_valve:ventricle"
inflow_data_original = data_original[mask_in_original]
inflow_data_original[:,2] = inflow_data_original[:,2].astype(float)*m3s_to_mLs




fig, axs1 = plt.subplots(1, 1, figsize=(6,8))

axs1.plot(volume_data_original[:,2].astype(float), p_ventricle_original_data[:,2].astype(float), label='New Viscosity')

axs1.set_xlabel('Ventricle Volume [ml]')
axs1.set_ylabel('Ventricle Pressure [mmHg]')
axs1.set_title('Pressure-Volume Loop of Original Ventricle')
axs1.legend()
axs1.grid()
# plt.savefig('pv_loop_expMat_correctMat-1.pdf')


# Create a new figure with 4x1 subplots
fig, axs = plt.subplots(3, 1, figsize=(10, 12))


# Single column (Original data)
axs[0].plot(p_in_original_data[:,1].astype(float), p_in_original_data[:,2].astype(float), label='Upstream Vessel Pressure New Viscosity')
axs[0].plot(p_ventricle_original_data[:,1].astype(float), p_ventricle_original_data[:,2].astype(float), label='Ventricular Pressure New Viscosity')
axs[0].plot(p_out_original_data[:,1].astype(float), p_out_original_data[:,2].astype(float), label='Downstream Vessel Pressure New Viscosity')

axs[0].legend()
axs[0].grid()

axs[1].plot(volume_data_original[:,1].astype(float), volume_data_original[:,2].astype(float), label='New Viscosity')

axs[1].legend()
axs[1].grid()

axs[2].plot(inflow_data_original[:,1].astype(float), inflow_data_original[:,2].astype(float), label='Inflow through Inlet Valve New Viscosity')
axs[2].plot(outflow_data_original[:,1].astype(float), outflow_data_original[:,2].astype(float), label='Outflow through Outlet Valve New Viscosity')

axs[2].legend()
axs[2].grid()

# Add x-label to bottom subplot
axs[2].set_xlabel('Time [s]')

# plt.savefig('Wiggers_comparison_expMat_correctMat-1.pdf')
plt.show()