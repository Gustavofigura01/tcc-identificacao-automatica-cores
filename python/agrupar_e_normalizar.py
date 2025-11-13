import pandas as pd
import glob, os

# Configurações
pasta_csv   = './'                                # Diretório onde estão os arquivos
padrao      = os.path.join(pasta_csv, 'dados_*.csv')
arquivo_out = 'dataset_cores.csv'                 # Nome do arquivo de saída

# Agrupar CSVs
arquivos = glob.glob(padrao)
print(f"🔎 Encontrados {len(arquivos)} arquivos...")

dfs = [pd.read_csv(f) for f in arquivos]
df_total = pd.concat(dfs, ignore_index=True)
print(f"📊 Total de amostras combinadas: {len(df_total)}")

# Salvar agrupado
df_total.to_csv(arquivo_out, index=False)
print(f"✅ Arquivo combinado salvo como: {arquivo_out}")
