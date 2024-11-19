
import styles from "./ChartsContainer.module.css";
import Chart from "../Chart/Chart";
import { humidityData, temperatureData } from "../../utils/mockdata";

const ChartsContainer = () => {
    return (
        <div className={styles.container} >
            <Chart data={humidityData} name={"Humidity"} dataKey={"humidity"} />
            <Chart data={temperatureData} name={"Temperature"} dataKey={"temp"} />
            <Chart data={humidityData} name={"Humidity"} dataKey={"humidity"} />
        </div>
    )
}

export default ChartsContainer;