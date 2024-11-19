import { useState } from "react";
import styles from "./ControlButton.module.css";
import axios from "axios";

const arduinoIp = '192.168.230.175';

const ControlButton = ({ name, id, status }) => {
    const [ state, setState ] = useState(status);

    const toggleFunction = async () => {
        const newState = !state;
        const res = await post(newState);
        if(res) setState(newState); // if res is true, update state
    }

    const post = async (newState) => {
        let action = newState ? 'turn_on' : 'turn_off';

        try {
            const response = await axios.post(`${arduinoIp}/${id}`, {
                action: action,
            });
            return true;
        } catch (error) {
            console.error(error);
            return false;
        }
    }

    return (
        <button
            id={id}
            className={styles.button}
            onClick={toggleFunction}
        >
            <div>
                <p>{name}</p>     
            </div> 
            <div className={state ? styles.on : styles.off} >
                <p>{state ? 'On' : 'Off'}</p>
            </div>
        </button>
    )
}

export default ControlButton;
