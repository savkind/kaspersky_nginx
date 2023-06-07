#include <ngx_http.h>
#include <i2c_driver.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>

typedef struct {
    ngx_str_t id;
    ngx_str_t address;
} component_t;

static component_t component_addresses[100]; 
static ngx_int_t num_components = 0;

static ngx_int_t parse_config_file(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        return NGX_ERROR; 
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {

        char* token = strtok(line, ";");
        if (token == NULL) {
            continue; 
        }
        ngx_str_t id = ngx_string(token);

        token = strtok(NULL, ";");
        if (token == NULL) {
            continue;
        }
        ngx_str_t address = ngx_string(token);

        // Добавление компонента в массив
        component_addresses[num_components].id = id;
        component_addresses[num_components].address = address;
        num_components++;
    }

    fclose(file);
    return NGX_OK;
}

static char *ngx_http_smart_home_handler(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_smart_home_handler;
    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_smart_home_handler(ngx_http_request_t *r)
{
    if (r->method != NGX_HTTP_GET)
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    ngx_str_t response = ngx_string("OK");

    ngx_int_t rc = ngx_http_send_header(r);
    if (rc != NGX_OK)
    {
        return rc;
    }

    rc = ngx_http_output_filter(r, &response);
    if (rc != NGX_OK)
    {
        return rc;
    }

    ngx_str_t id, action;
    if (ngx_http_arg(r, (u_char *)"id", 2, &id) != NGX_OK ||
        ngx_http_arg(r, (u_char *)"action", 6, &action) != NGX_OK)
    {
        return NGX_HTTP_BAD_REQUEST;
    }


    ngx_int_t parseConfigResult = parse_config_file("config_address");
    if (parseConfigResult != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    char component_address[64];
    int i;
    for (i = 0; i < num_components; i++) {
        if (strcmp((const char*)component_addresses[i].id.data, (const char*)id.data) == 0) {
            strcpy(component_address, (const char*)component_addresses[i].address.data);
            break;
        }
    }

    if (i == num_components) {
        return NGX_HTTP_BAD_REQUEST;
    }

    i2c_send_command(component_address, (const char*)action.data);
    ngx_str_t i2c_response = i2c_receive_response();
    if (ngx_strcmp(i2c_response.data, "OK") != 0)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_str_t parameter;
    if (ngx_http_arg(r, (u_char *)"parameter", 9, &parameter) == NGX_OK)
    {
        ngx_int_t sendParamResult = sendDeviceParameter(&id, &parameter);
        if (sendParamResult != NGX_HTTP_OK)
        {
            return sendParamResult;
        }
    }

    return NGX_HTTP_OK;
}

static ngx_int_t sendDeviceParameter(ngx_str_t * id, ngx_str_t * parameter)
{
    char component_address[64];
    int i;
    for (i = 0; i < num_components; i++) {
        if (strcmp((const char*)component_addresses[i].id.data, (const char*)id->data) == 0) {
            strcpy(component_address, (const char*)component_addresses[i].address.data);
            break;
        }
    }

    if (i == num_components) {
        return NGX_HTTP_BAD_REQUEST;
    }

  
    i2c_send_command(component_address, (const char*)parameter);
    ngx_str_t i2c_response = i2c_receive_response();
    if (ngx_strcmp(i2c_response.data, "OK") != 0)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    return NGX_HTTP_OK;
}
void i2c_send_command(const char* address, const char* command) {
    int file;
    char filename[20];
    sprintf(filename, "/dev/i2c-1");  

    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the I2C bus");
        return;
    }

    int addr = strtol(address, NULL, 16);  // Преобразование адреса из строкового представления в целое число


    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return;
    }

  
    if (write(file, command, strlen(command)) != strlen(command)) {
        perror("Failed to write to the I2C bus");
        return;
    }

    close(file);
}