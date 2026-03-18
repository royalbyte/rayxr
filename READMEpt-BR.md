# rayxr

Um wrapper OpenXR minimalista para o [raylib](https://www.raylib.com/).  
Escreva aplicações VR usando a API de desenho do raylib sem lidar com o OpenXR diretamente.

---

## O que é

O rayxr cuida do boilerplate do OpenXR — instância, sessão, swapchains, frame loop, pose tracking — e expõe 12 funções para você desenhar em VR da mesma forma que desenha numa tela normal com raylib.

Não é um framework. Não abstrai input, controles, overlays ou layers além do mínimo necessário para renderização estéreo. Se precisar disso, você tem acesso direto aos handles `XrSession` e `XrInstance` subjacentes.

---

## Dependências

- [raylib](https://github.com/raysan5/raylib) >= 5.0
- [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK) >= 1.0
- OpenGL (via GLX, X11)
- Um runtime OpenXR rodando (ex: [Monado](https://monado.dev/), SteamVR, Oculus)

---

## Como compilar

Compile o rayxr em um arquivo objeto e linke com seu projeto:

```sh
# compilar o rayxr
gcc -c src/rayxr_core.c src/rayxr_session.c src/rayxr_render.c src/rayxr_pose.c \
    -Iinclude -I/usr/include/openxr \
    -o rayxr.o

# compilar e linkar seu projeto
gcc seu_app.c rayxr.o \
    -Iinclude \
    -lraylib -lopenxr_loader -lGL -lX11 \
    -o seu_app
```

Ou adicione os fontes diretamente ao seu sistema de build — não há nenhum requisito de sistema de build.

---

## Uso

```c
#include <raylib.h>
#include <openxr/openxr.h>
#include <rayxr/rayxr.h>

int main(void)
{
    /* a janela do raylib deve ser criada antes do rxr_init */
    InitWindow(800, 600, "minha app vr");

    if (rxr_init())
        return 1;

    while (!WindowShouldClose() && rxr_is_running()) {
        if (rxr_begin_frame())
            continue;

        Camera3D cam = rxr_get_head_camera();

        rxr_begin_eye(0);
            ClearBackground(RAYWHITE);
            BeginMode3D(cam);
                /* desenhe sua cena */
            EndMode3D();
        rxr_end_eye();

        rxr_begin_eye(1);
            ClearBackground(RAYWHITE);
            BeginMode3D(cam);
                /* desenhe sua cena */
            EndMode3D();
        rxr_end_eye();

        rxr_end_frame();
    }

    rxr_shutdown();
    CloseWindow();
    return 0;
}
```

A ordem dos includes importa — raylib e openxr devem ser incluídos antes do rayxr.h.

---

## API

| Função | Descrição |
|---|---|
| `rxr_init()` | Inicializa o OpenXR. Chame após `InitWindow()`. Retorna 0 em caso de sucesso. |
| `rxr_shutdown()` | Destrói todos os recursos OpenXR. Chame antes de `CloseWindow()`. |
| `rxr_begin_frame()` | Aguarda o frame, inicia o frame e localiza as views. Retorna 1 se não estiver pronto. |
| `rxr_end_frame()` | Envia os olhos renderizados para o runtime OpenXR. |
| `rxr_begin_eye(int eye)` | Vincula o framebuffer e viewport para o olho 0 (esquerdo) ou 1 (direito). |
| `rxr_end_eye()` | Libera a imagem do swapchain do olho atual. |
| `rxr_get_head_camera()` | Retorna uma `Camera3D` do raylib a partir da pose atual da cabeça. |
| `rxr_get_head_pose()` | Retorna o `XrPosef` bruto da pose atual da cabeça. |
| `rxr_is_running()` | Retorna 1 enquanto a sessão XR estiver ativa. |
| `rxr_session()` | Retorna o handle `XrSession` subjacente. |
| `rxr_instance()` | Retorna o handle `XrInstance` subjacente. |

---

## Limitações conhecidas

- **Somente Linux/X11.** O binding OpenGL usa GLX (`XR_KHR_opengl_enable`). Windows (WGL) e Wayland não são suportados.
- **Sem câmera por olho.** `rxr_get_head_camera()` retorna uma única câmera a partir da pose do olho esquerdo. Para estéreo fisicamente preciso, os dois olhos têm posições e FOVs ligeiramente diferentes — o `Camera3D` do raylib não modela isso. Para a maioria dos casos de uso a diferença é imperceptível.
- **Sem envio de profundidade.** O frame é enviado como uma composition layer somente de cor. Alguns runtimes usam profundidade para reprojeção — isso não é suportado.
- **Sem input de controle.** O rayxr não encapsula o input do OpenXR. Use `rxr_session()` e a API de input do OpenXR diretamente se precisar.
- **Imagens de swapchain estáticas.** O número de imagens do swapchain é assumido como 4 ou menos. Isso vale para todos os runtimes conhecidos mas não é garantido pela spec.