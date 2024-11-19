import {LineChart, CartesianGrid, XAxis, YAxis, Tooltip, Legend,Line} from "recharts"
import styles from "./Chart.module.css"

const Chart = ({ data, name, dataKey }) => {

    return (
        <div className={styles.container}>
            <div className={styles.header} >
                <p className={styles.title} >{name}</p>
            </div>
                <LineChart width={720} height={200} data={data} margin={{ top: 5, right: 30, left: 20, bottom: 5 }}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="name" />
                    <YAxis />
                    <Tooltip />
                    <Legend />
                    <Line type="monotone" dataKey={dataKey} stroke="#82ca9d" />
                </LineChart>
        </div>
    )

}

export default Chart;