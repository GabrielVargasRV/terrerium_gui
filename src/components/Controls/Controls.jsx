import style from "./Controls.module.css";
import ControlButton from "../ControlButton/ControlButton";

const Controls = () => {

    return (
        <div className={style.container} >
            <ControlButton id={'fan1'} name={'Fan 1'} status={true} />
            <ControlButton id={'fan2'} name={'Fan 2'} status={false} />
            <ControlButton id={'light1'} name={'Light 1'} status={true} />
            <ControlButton id={'light2'} name={'Light 2'} status={false} />
            <ControlButton id={'pump'} name={'Pump'} status={true} />
        </div>
    )
}

export default Controls;