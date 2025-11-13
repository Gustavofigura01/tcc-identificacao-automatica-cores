# Documentação do Projeto

Este diretório contém toda a documentação técnica e científica do projeto.

## Estrutura Sugerida

```
docs/
├── monografia/
│   ├── monografia.tex        # Documento principal LaTeX
│   ├── capitulos/            # Capítulos da monografia
│   ├── figuras/              # Imagens e gráficos
│   └── referencias.bib       # Referências bibliográficas
├── apresentacao/
│   ├── slides.tex            # Apresentação de defesa
│   └── imagens/              # Imagens da apresentação
├── relatorios/
│   └── progresso_*.pdf       # Relatórios de progresso
├── diagramas/
│   ├── arquitetura.png       # Diagrama de arquitetura
│   ├── fluxo_dados.png       # Fluxo de dados
│   └── uml/                  # Diagramas UML
└── especificacoes/
    ├── hardware.md           # Especificações de hardware
    ├── software.md           # Especificações de software
    └── protocolo_comm.md     # Protocolo de comunicação
```

## Conteúdo

### Monografia
- Introdução e justificativa
- Revisão bibliográfica
- Metodologia
- Desenvolvimento
- Resultados e discussão
- Conclusão
- Referências

### Apresentação
- Slides para defesa do TCC
- Roteiro de apresentação
- Demonstrações práticas

### Especificações Técnicas

#### Hardware
- Especificações do sensor CCD
- Pinout do ESP32
- Esquemático eletrônico
- Lista de componentes (BOM)

#### Software
- Arquitetura do sistema
- APIs e protocolos
- Fluxogramas
- Diagramas de classe

#### Machine Learning
- Arquitetura da rede neural
- Métricas de desempenho
- Matriz de confusão
- Curvas de aprendizado

## Ferramentas Recomendadas

- **LaTeX:** Overleaf, TeXstudio, ou VS Code com LaTeX Workshop
- **Diagramas:** Draw.io, Lucidchart, ou PlantUML
- **Gestão de Referências:** Zotero, Mendeley, ou JabRef
- **Controle de Versão:** Git (já configurado!)

## Templates

Para uniformizar a documentação, utilize os templates disponibilizados pela UTFPR ou crie seus próprios baseados nas normas ABNT.

## Compilação LaTeX

```bash
# Compilar monografia
cd monografia
pdflatex monografia.tex
bibtex monografia
pdflatex monografia.tex
pdflatex monografia.tex

# Ou use latexmk para automação
latexmk -pdf monografia.tex
```

## Dicas de Escrita

1. **Seja claro e objetivo**
2. **Use linguagem técnica apropriada**
3. **Documente todas as decisões de projeto**
4. **Inclua tabelas e gráficos quando necessário**
5. **Cite todas as fontes utilizadas**
6. **Revise ortografia e gramática**
7. **Peça feedback do orientador regularmente**

## Cronograma

Mantenha aqui um cronograma atualizado das atividades:

- [ ] Revisão bibliográfica
- [ ] Projeto de hardware
- [ ] Implementação do firmware
- [ ] Coleta de dados
- [ ] Treinamento do modelo
- [ ] Desenvolvimento do app
- [ ] Testes e validação
- [ ] Escrita da monografia
- [ ] Preparação da apresentação
- [ ] Defesa do TCC

## Contato com Orientador

Registre aqui informações de contato e horários de orientação:

- **Orientador:** [Nome do Professor]
- **Email:** [email]
- **Horário de atendimento:** [dias e horários]
