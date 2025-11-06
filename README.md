# Decriptação de cifras clássicas

Esse projeto implementa ferramentas pra analisar e potencialmente quebrar a cifras clássica monoalfabética do ramo da criptografia.

## Estrutura

- `textos_conhecidos/` - Diretório com cifras retiradas de textos conhecidos
- `textos_desconhecidos/` - Diretório com cifras retiradas de textos desconhecidos

## Compilação e execução 

Comandos para linux.

```bash
cd ClassicalCiphersDecryption/textos_conhecidos
```

compilação:
```bash
gcc hack_monoalphabetic_cipher.c -o textos_conhecidos/hack_cipher.exe
```

Execução:
```bash
./hack_cipher.exe
```
## Requisitos

- Compilador GCC
- Bibliotecas padrões de C 
