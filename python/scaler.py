import joblib, numpy as np

scaler = joblib.load('improved_color_classifier_scaler.pkl')
mu     = scaler.mean_.astype(np.float32)
sigma  = scaler.scale_.astype(np.float32)

with open('scaler.h', 'w') as f:
    f.write('#pragma once\n\n')
    f.write('static const float INPUT_MEAN[1577] = {\n  ')
    f.write(', '.join(f'{x:.8e}' for x in mu))
    f.write('\n};\n\n')
    f.write('static const float INPUT_STD[1577] = {\n  ')
    f.write(', '.join(f'{x:.8e}' for x in sigma))
    f.write('\n};\n')
print('Gerado scaler.h (≈12 kB)')
