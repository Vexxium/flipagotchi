#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>

static uint8_t hunger = 100;
static uint8_t boredom = 0;
static uint32_t last_tick;

static NotificationApp* notifications;

static void draw_callback(Canvas* canvas, void* ctx) {
    canvas_clear(canvas);
    canvas_draw_str(canvas, 10, 10, "CHAOS PET");

    if(hunger == 0) {
        canvas_draw_str(canvas, 30, 30, "(X_X)");
    } else if(boredom > 70) {
        canvas_draw_str(canvas, 30, 30, "(>_<)");
    } else if(hunger < 30) {
        canvas_draw_str(canvas, 30, 30, "(ಠ_ಠ)");
    } else {
        canvas_draw_str(canvas, 30, 30, "(^_^)");
    }

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "H:%d B:%d", hunger, boredom);
    canvas_draw_str(canvas, 10, 55, buffer);
}

static void input_callback(InputEvent* event, void* ctx) {
    if(event->type != InputTypePress) return;

    if(event->key == InputKeyOk) {
        if(hunger < 100) hunger += 15;
        notification_message(notifications, &sequence_blink_green_100);
    }

    if(event->key == InputKeyLeft) {
        if(boredom > 0) boredom -= 20;
        notification_message(notifications, &sequence_blink_blue_100);
    }
}

int32_t chaos_pet_app(void* p) {
    UNUSED(p);
    last_tick = furi_get_tick();
    notifications = furi_record_open(RECORD_NOTIFICATION);

    Gui* gui = furi_record_open("gui");
    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, NULL);

    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(1) {
        if(furi_get_tick() - last_tick > 3000) {
            if(hunger > 0) hunger--;
            if(boredom < 100) boredom++;
            if(boredom > 80) {
                notification_message(notifications, &sequence_double_beep);
            }
            last_tick = furi_get_tick();
        }
        furi_delay_ms(100);
    }

    view_port_free(view_port);
    furi_record_close("gui");
    furi_record_close(RECORD_NOTIFICATION);

    return 0;
}