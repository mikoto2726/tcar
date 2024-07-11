#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#define SENSOR_MAX 4
#define SNS_TH 10

#define PMD_DATA_EN 0
#define PMD_DATA_PERIOD 1
#define PMD_DATA_PULSE0 2
#define PMD_DATA_PULSE1 3
#define PMD_DATA_PULSE2 4

typedef struct
{
    UH clear;
    UH red;
    UH green;
    UH blue;
} T_COLOR_DATA;

const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};
T_COLOR_DATA cdata[SENSOR_MAX];
INT diff, steer;

// sensor .
ER init_BH1745NUC(ID dd)
{
    UB data[2];
    SZ asz;
    ER err;

    for(INT i = 0; i < SENSOR_MAX; i++)
    {
        data[0] = 0x40; // SYSTEM_CONTROL register .
        data[1] = 0x80;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);
        if (err < E_OK)
        {
            tm_printf((UB*)"Error in initializing sensor %d: %d\n", i, err);
            return err;
        }

        data[0] = 0x40; // SYSTEM_CONTROL register .
        data[1] = 0x00;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);
        if (err < E_OK)
        {
            tm_printf((UB*)"Error in starting sensor %d: %d\n", i, err);
            return err;
        }

        data[0] = 0x42; // SYSTEM_CONTROL2 register .
        data[1] = 0x10;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);
        if (err < E_OK)
        {
            tm_printf((UB*)"Error in enabling sensor %d: %d\n", i, err);
            return err;
        }
    }
    return err;
}

ER read_sensor(ID dd, T_COLOR_DATA cdata[])
{
    SZ asz;
    ER err;
    INT i;

    // Getting sensor readings .
    for (i = 0; i < SENSOR_MAX; i++)
    {
        while (1)
        {
            T_I2C_EXEC exec;
            UB snd_data;
            UB rcv_data;
            
            // reading MODE_CONTROL2 register .
            exec.sadr = sensor_addr[i];
            snd_data = 0x42;
            exec.snd_size = 1;
            exec.snd_data = &snd_data;
            exec.rcv_size = 1;
            exec.rcv_data = &rcv_data;

            err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
            if (err < E_OK)
            {
                tm_printf((UB*)"Error in I2C execution for sensor %d: %d\n", i, err);
                return err;
            }
            
            if (rcv_data & (1 << 7))
            {
                // reading sensor data .
                UH sens_data[4];

                exec.sadr = sensor_addr[i];
                snd_data = 0x50;
                exec.snd_size = 1;
                exec.snd_data = &snd_data;
                exec.rcv_size = sizeof(sens_data);
                exec.rcv_data = (UB*)sens_data;
                
                err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
                if (err < E_OK)
                {
                    tm_printf((UB*)"Error in reading sensor data %d: %d\n", i, err);
                    return err;
                }

                cdata[i].red = sens_data[0];
                cdata[i].green = sens_data[1];
                cdata[i].blue = sens_data[2];
                cdata[i].clear = sens_data[3];

                // debug .
                // tm_printf((UB*)"Sensor %d - Clear: %d\n", i, cdata[i].clear);
                break;
            }
        }
    }
    return err;
}

//steer settings .
ER init_pmd(ID dd)
{
    UW data;
    SZ asz;
    ER err;

    data = 14000; // Waveform period 14,000 microseconds .
    err = tk_swri_dev(dd, PMD_DATA_PERIOD, &data, 1, &asz);
    if (err < E_OK)
    {
        tm_printf((UB*)"Error in setting PMD_DATA_PERIOD: %d\n", err);
        return err;
    }

    // Initial value of the driving motor (stopped) .
    data = 14000 - 1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE0, &data, 1, &asz);
    if (err < E_OK)
    {
        tm_printf((UB*)"Error in setting PMD_DATA_PULSE0: %d\n", err);
        return err;
    }

    // Initial value of the steering (straight) .
    data = 1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);
    if (err < E_OK)
    {
        tm_printf((UB*)"Error in setting PMD_DATA_PULSE1: %d\n", err);
        return err;
    }

    // Output start .
    data = 1;
    err = tk_swri_dev(dd, PMD_DATA_EN, &data, 1, &asz);
    if (err < E_OK)
    {
        tm_printf((UB*)"Error in setting PMD_DATA_EN: %d\n", err);
        return err;
    }

    return E_OK;
}

ER set_steer(ID dd, INT steer)
{
    UW data;
    SZ asz;
    ER err;

    if(steer < -500 || steer > 500)
        return E_PAR;

    // Map the range from -500 to +500 to the range of 1,000 to 2,000 .
    data = steer + 1500;
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);
    if (err < E_OK)
        tm_printf((UB*)"Error in setting steer: %d\n", err);
    
    return err;
}

LOCAL void drv_task(void *exinf)
{
    ID sns_dd, pmd_dd;
    ER err;

    sns_dd = tk_opn_dev((UB*)"iicb", TD_UPDATE);
    if (sns_dd < E_OK)
    {
        tm_printf((UB*)"Failed to open iicb: %d\n", sns_dd);
        return ;
    }
    
    err = init_BH1745NUC(sns_dd);
    if (err < E_OK)
        return ;

    pmd_dd = tk_opn_dev((UB*)"pmda", TD_UPDATE);
    if (pmd_dd < E_OK)
    {
        tm_printf((UB*)"Failed to open pmda: %d\n", pmd_dd);
        tk_cls_dev(sns_dd, 0);
        return ;
    }

    err = init_pmd(pmd_dd);
    if (err < E_OK)
    {
        tk_cls_dev(pmd_dd, 0);
        return ;
    }

    while (1)
    {
        err = read_sensor(sns_dd, cdata);
        if (err < E_OK)
        {
            tm_printf((UB*)"Error in reading sensors: %d\n", err);
            tk_dly_tsk(100);
            continue ;
        }

        diff = (cdata[2].clear + cdata[3].clear) - (cdata[0].clear + cdata[1].clear);

        if (diff > SNS_TH)
            steer = 500;
        else if (diff < -SNS_TH)
            steer = -500;
        else
            steer = 0;
        set_steer(pmd_dd, steer);

        tk_dly_tsk(100);
    }
}

EXPORT INT usermain(void)
{
    T_CTSK drv_ctsk;
    ID drv_tskid;

    drv_ctsk.tskatr = TA_HLNG | TA_RNG3;
    drv_ctsk.task = (FP)drv_task;
    drv_ctsk.itskpri = 10;
    drv_ctsk.stksz = 1024;

    drv_tskid = tk_cre_tsk(&drv_ctsk);
    if (drv_tskid < E_OK)
    {
        tm_printf((UB*)"Failed to create driver task: %d\n", drv_tskid);
        return 1;
    }
    tk_sta_tsk(drv_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;
}
