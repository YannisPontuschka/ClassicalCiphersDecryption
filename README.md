# Decriptação de cifras clássicas

Esse projeto implementa ferramentas pra analisar e potencialmente quebrar cifras clássicas do ramo da criptografia.

## Project Structure

- `textos_conhecidos/` - Diretório com cifras retiradas de textos conhecidos
- `textos_desconhecidos/` - Diretório com cifras retiradas de textos desconhecidos

## Compilação e execução para Cifra Monoalfabética com texto conhecido

Comandos para linux.

compilação:
```bash
gcc textos_conhecidos/hack_monoalphabetic.c -o textos_conhecidos/hack_cipher
```

Execução:
```bash
cd textos_conhecidos && ./hack_cipher
```
## Requisitos

- Compilador GCC
- Bibliotecas padrões de C 