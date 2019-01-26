using UnityEngine;
using System.Collections;
using UnityStandardAssets.CrossPlatformInput;

namespace UnityStandardAssets.Vehicles.Car
{
    [RequireComponent(typeof(CarController))]
    public class CarKeyMouseController : MonoBehaviour
    {

        private CarController m_Car;

        private void Start()
        {

            // get the car controller
            m_Car = GetComponent<CarController>();
            Debug.Log("CarKeyMouseController Start ");
        }

        // Use this for initialization
        void OnEnable()
        {
            CarUserControl.OnAction += KeyAndMouseControl;
            Debug.Log("CarKeyMouseController OnEnable ");
        }

        // Update is called once per frame
        void OnDisable()
        {
            CarUserControl.OnAction -= KeyAndMouseControl;
            Debug.Log("CarKeyMouseController OnDisable ");
        }


        private void KeyAndMouseControl()
        {

            float h = 0f;
            float v = 0f;
            if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.RightArrow)
                    || Input.GetKey(KeyCode.UpArrow) || Input.GetKey(KeyCode.DownArrow))
            {
                Debug.Log(" Voice Control ");
                if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.RightArrow))
                {
                    h = CrossPlatformInputManager.GetAxis("Horizontal");
                }
                else
                {
                    h = CrossPlatformInputManager.GetAxis("Mouse X");
                }
                
                v = CrossPlatformInputManager.GetAxis("Vertical");
                
                //m_Car.Move(h, v, v, 0f);
            }
            else //if (Input.GetKey(KeyCode.Mouse0) || Input.GetKey(KeyCode.Mouse1))
            {
                Debug.Log(" Mouse Control ");

                if (Input.GetKey(KeyCode.LeftArrow) || Input.GetKey(KeyCode.RightArrow))
                {
                    h = CrossPlatformInputManager.GetAxis("Horizontal");
                }
                else
                {
                    h = CrossPlatformInputManager.GetAxis("Mouse X");
                }
                v = CrossPlatformInputManager.GetAxis("Mouse Y");
            
            }

            m_Car.Move(h, v, v, 0f);
        }
    }
}
