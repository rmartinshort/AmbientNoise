/* header file for disp_select.c code: select station pair correlations for creating a dispersion map */
  struct corr_pr{              /* structure for single correlation pair */
    char sta1_name[40];
    char sta2_name[40];
    char pos_neg;
    float sta1lat;
    float sta1lon;
    float sta2lat;
    float sta2lon;
    float snr_temp;
    float snr_spect;
    float gp_veloc;
    float ph_veloc;
  };

  typedef struct all_stas{              /* structure to hold corr_pr array */
    struct corr_pr fname[200000];
    int len;                    /* total number of files in a given directory */
  }
  CORRELATION_PAIRS;

