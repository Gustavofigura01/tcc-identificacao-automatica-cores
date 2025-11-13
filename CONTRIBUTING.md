# Guia de Contribuição

## Convenções de Commit

Este projeto segue o padrão de [Conventional Commits](https://www.conventionalcommits.org/).

### Formato

```
<tipo>(<escopo>): <descrição curta>

<corpo opcional>

<rodapé opcional>
```

### Tipos de Commit

- **feat**: Nova funcionalidade
- **fix**: Correção de bug
- **docs**: Alterações na documentação
- **style**: Formatação de código (sem alteração de lógica)
- **refactor**: Refatoração de código
- **test**: Adição ou correção de testes
- **chore**: Tarefas de manutenção (dependências, config, etc.)
- **perf**: Melhorias de performance
- **build**: Alterações no sistema de build
- **ci**: Configurações de CI/CD

### Exemplos

```bash
# Nova funcionalidade
git commit -m "feat(firmware): adiciona suporte a BLE para comunicação"

# Correção de bug
git commit -m "fix(python): corrige erro na normalização dos dados do sensor"

# Documentação
git commit -m "docs(readme): atualiza instruções de instalação"

# Refatoração
git commit -m "refactor(matlab): melhora legibilidade do script de captura"

# Performance
git commit -m "perf(firmware): otimiza loop de inferência TFLite"
```

## Fluxo de Trabalho

### Branches

- **main**: Código estável e pronto para produção
- **develop**: Branch de desenvolvimento
- **feature/[nome]**: Novas funcionalidades
- **fix/[nome]**: Correções de bugs
- **docs/[nome]**: Atualizações de documentação

### Criando uma Branch

```bash
# Para nova funcionalidade
git checkout -b feature/sensor-temperatura

# Para correção de bug
git checkout -b fix/erro-bluetooth

# Para documentação
git checkout -b docs/api-reference
```

### Mesclando Branches

```bash
# Voltar para main
git checkout main

# Mesclar a branch
git merge feature/sua-feature

# Ou fazer rebase
git rebase feature/sua-feature
```

## Boas Práticas

### Commits

1. **Commits atômicos**: Um commit deve representar uma única mudança lógica
2. **Mensagens descritivas**: Explique o "porquê", não apenas o "o quê"
3. **Commits frequentes**: Faça commits regularmente
4. **Não commite arquivos gerados**: Use o .gitignore adequadamente

### Código

1. **Comente código complexo**
2. **Mantenha consistência de estilo**
3. **Teste antes de commitar**
4. **Atualize a documentação quando necessário**

### Python

```python
# Siga PEP 8
# Use type hints
def processar_dados(entrada: np.ndarray) -> np.ndarray:
    """
    Processa dados do sensor.
    
    Args:
        entrada: Array com dados brutos do sensor
        
    Returns:
        Array com dados processados
    """
    return entrada * 2.0
```

### C++ (ESP32)

```cpp
// Use CamelCase para classes
// Use snake_case para funções e variáveis
// Documente funções complexas

class SensorCCD {
public:
    void inicializar();
    float* ler_dados();
    
private:
    int numero_pixels;
};
```

## Checklist Antes de Commitar

- [ ] Código compila sem erros
- [ ] Testes passam (se aplicável)
- [ ] Documentação atualizada
- [ ] .gitignore configurado corretamente
- [ ] Sem arquivos temporários ou gerados
- [ ] Mensagem de commit descritiva
- [ ] Formatação de código consistente

## Versionamento

Este projeto segue [Semantic Versioning](https://semver.org/):

- **MAJOR**: Mudanças incompatíveis na API
- **MINOR**: Novas funcionalidades (compatível)
- **PATCH**: Correções de bugs

Exemplo: `v1.2.3`

## Tags

Para marcar releases:

```bash
git tag -a v1.0.0 -m "Primeira versão estável"
git push origin v1.0.0
```

## Ignorar Arquivos

Certifique-se de que os seguintes arquivos **nunca** sejam commitados:

- ❌ Modelos treinados (*.h5, *.pkl)
- ❌ Datasets (*.csv)
- ❌ Arquivos compilados (*.bin, *.elf)
- ❌ Credenciais e senhas
- ❌ Arquivos temporários do IDE

## Dúvidas?

Abra uma issue no repositório ou entre em contato com o mantenedor.

---

**Obrigado por contribuir!** 🚀
