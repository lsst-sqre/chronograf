import {TimeRangeOption} from 'src/types/queries'

const nowMinus30d = 'now() - 30d'

export const timeRanges: TimeRangeOption[] = [
  {
    defaultGroupBy: '1m',
    seconds: 900,
    inputValue: 'Past 15m',
    lower: 'now() - 15m',
    lowerFlux: '-15m',
    upper: null,
    menuOption: 'Past 15m',
  },
  {
    defaultGroupBy: '1m',
    seconds: 3600,
    inputValue: 'Past 1h',
    lower: 'now() - 1h',
    lowerFlux: '-1h',
    upper: null,
    menuOption: 'Past 1h',
  },
  {
    defaultGroupBy: '10m',
    seconds: 86400,
    inputValue: 'Past 24h',
    lower: 'now() - 24h',
    lowerFlux: '-24h',
    upper: null,
    menuOption: 'Past 24h',
  },
  {
    defaultGroupBy: '1h',
    seconds: 604800,
    inputValue: 'Past 7d',
    lower: 'now() - 7d',
    lowerFlux: '-7d',
    upper: null,
    menuOption: 'Past 7d',
  },
  {
    defaultGroupBy: '12h',
    seconds: 2592000,
    inputValue: 'Past 30d',
    lower: nowMinus30d,
    lowerFlux: '-30d',
    upper: null,
    menuOption: 'Past 30d',
  },
{
    defaultGroupBy: '1d',
    seconds: 22118400,
    inputValue: 'Past year',
    lower: 'now()-356d',
    lowerFlux: '-356d',
    upper: null,
    menuOption: 'Past year',
  },
]

export const FORMAT_INFLUXQL = 'influxql'

export const DEFAULT_TIME_RANGE = {
  upper: null,
  lower: nowMinus30d,
  lowerFlux: '-30d',
  seconds: 2592000,
  format: FORMAT_INFLUXQL,
}

export const STATUS_PAGE_TIME_RANGE = timeRanges.find(
  tr => tr.lower === nowMinus30d
)
