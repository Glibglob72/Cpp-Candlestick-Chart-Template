# Data folder

Put OHLCV 1-minute CSV files here — every `.csv` in this folder appears in the app's file dropdown.

Expected format (header required):

```
date,time,open,high,low,close,volume
2024-01-15,09:30:00,4800.25,4805.50,4799.00,4803.75,1250
```

- **Sample data:** building the project generates `sample_ohlcv_1m.csv` automatically (see the main README), so you can try the app without downloading anything.
- **Real market data:** historical futures OHLCV 1-minute bars (e.g. CME Micro E-mini S&P 500 `MES`, Micro E-mini Nasdaq-100 `MNQ`) are available from [Databento](https://databento.com). Export as CSV and reorder/rename columns to match the format above if needed.

Market data is licensed by its vendor — do not commit purchased data to the repository.
